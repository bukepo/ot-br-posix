#include "filter_loader.hpp"
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include <errno.h>
#include <linux/bpf.h>
#include <linux/if_link.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>

#include <openthread/backbone_router_ftd.h>
#include <openthread/border_routing.h>
#include <openthread/ip6.h>
#include <openthread/netdata.h>
#include <openthread/thread.h>

#include "filter_bytecode.h"
#include "common/logging.hpp"

namespace otbr {
namespace bpf {

struct otbr_lpm_key
{
    uint32_t        prefixlen;
    struct in6_addr prefix;
};

static struct bpf_object *sBpfObj              = nullptr;
static int                sThreadPrefixesMapFd = -1;
static int                sAllowedDstMapFd     = -1;
static otInstance        *sInstance            = nullptr;
static const uint32_t kTcPriority          = 100;
static const uint32_t kTcHandle            = 1;
static bool               sIsAttached          = false;
static int                sAttachedIfIndex     = -1;

otError InitializeFilter(otInstance *aInstance)
{
    sInstance = aInstance;

    sBpfObj = bpf_object__open_mem(filter_o, filter_o_len, nullptr);
    if (!sBpfObj)
        return OT_ERROR_FAILED;

    if (bpf_object__load(sBpfObj) < 0)
    {
        bpf_object__close(sBpfObj);
        sBpfObj = nullptr;
        return OT_ERROR_FAILED;
    }

    sThreadPrefixesMapFd = bpf_object__find_map_fd_by_name(sBpfObj, "thread_prefixes");
    sAllowedDstMapFd     = bpf_object__find_map_fd_by_name(sBpfObj, "allowed_dst");

    if (sThreadPrefixesMapFd < 0 || sAllowedDstMapFd < 0)
    {
        bpf_object__close(sBpfObj);
        sBpfObj = nullptr;
        return OT_ERROR_NOT_FOUND;
    }
    return OT_ERROR_NONE;
}

void DeinitializeFilter(void)
{
    if (sBpfObj)
    {
        bpf_object__close(sBpfObj);
        sBpfObj = nullptr;
    }
    sThreadPrefixesMapFd = -1;
    sAllowedDstMapFd     = -1;
    sInstance            = nullptr;
}


static void ClearMap(int aMapFd)
{
    struct otbr_lpm_key nextKey;

    while (bpf_map_get_next_key(aMapFd, nullptr, &nextKey) == 0)
    {
        if (bpf_map_delete_elem(aMapFd, &nextKey) < 0)
        {
            otbrLogWarning("BPF Filter: Failed to delete key from map %d: %s", aMapFd, strerror(errno));
            break;
        }
    }
}

static void AddPrefixToMap(int aMapFd, const otIp6Prefix &aPrefix)
{
    struct otbr_lpm_key key;
    memset(&key, 0, sizeof(key));
    key.prefixlen = aPrefix.mLength;
    memcpy(&key.prefix, &aPrefix.mPrefix, sizeof(key.prefix));

    // Mask out unused bits for LPM trie
    int bytes = aPrefix.mLength / 8;
    int bits  = aPrefix.mLength % 8;
    if (bytes < 16)
    {
        if (bits > 0)
        {
            key.prefix.s6_addr[bytes] &= (0xFF << (8 - bits));
            bytes++;
        }
        memset(&key.prefix.s6_addr[bytes], 0, 16 - bytes);
    }

    uint32_t value = 1;
    if (bpf_map_update_elem(aMapFd, &key, &value, BPF_ANY) < 0)
    {
        otbrLogWarning("BPF Filter: Failed to update map %d: %s", aMapFd, strerror(errno));
    }
}

otError UpdateFilterPrefixes(void)
{
    if (sThreadPrefixesMapFd < 0 || sAllowedDstMapFd < 0 || !sInstance)
    {
        otbrLogWarning("BPF Filter: Cannot update prefixes, filter not initialized");
        return OT_ERROR_INVALID_STATE;
    }

    ClearMap(sThreadPrefixesMapFd);
    ClearMap(sAllowedDstMapFd);

    // 1. Get On-Mesh Prefixes from Network Data (matches firewall.cpp)
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    otBorderRouterConfig  config;
    while (otNetDataGetNextOnMeshPrefix(sInstance, &iterator, &config) == OT_ERROR_NONE)
    {
        AddPrefixToMap(sThreadPrefixesMapFd, config.mPrefix);
        AddPrefixToMap(sAllowedDstMapFd, config.mPrefix);

        char buf[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&config.mPrefix.mPrefix, buf, sizeof(buf));
        otbrLogInfo("BPF Filter: Added On-Mesh prefix %s/%d", buf, config.mPrefix.mLength);
    }

    // 2. Get OMR Prefix from Border Routing Manager
    otIp6Prefix omrPrefix;
    if (otBorderRoutingGetOmrPrefix(sInstance, &omrPrefix) == OT_ERROR_NONE && omrPrefix.mLength > 0)
    {
        AddPrefixToMap(sThreadPrefixesMapFd, omrPrefix);
        AddPrefixToMap(sAllowedDstMapFd, omrPrefix);

        char buf[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&omrPrefix.mPrefix, buf, sizeof(buf));
        otbrLogInfo("BPF Filter: Added OMR prefix %s/%d", buf, omrPrefix.mLength);
    }

    // 3. Get Mesh-Local Prefix
    const otMeshLocalPrefix *mlPrefix = otThreadGetMeshLocalPrefix(sInstance);
    if (mlPrefix)
    {
        otIp6Prefix prefix;
        memset(&prefix.mPrefix, 0, sizeof(prefix.mPrefix));
        memcpy(&prefix.mPrefix, mlPrefix, sizeof(*mlPrefix));
        prefix.mLength = 64; // ML prefix is always 64 bits
        AddPrefixToMap(sThreadPrefixesMapFd, prefix);

        char buf[OT_IP6_ADDRESS_STRING_SIZE];
        otIp6AddressToString(&prefix.mPrefix, buf, sizeof(buf));
        otbrLogInfo("BPF Filter: Added ML prefix %s/%d", buf, prefix.mLength);
    }



    return OT_ERROR_NONE;
}

otError AttachToInterface(const char *aInterfaceName)
{
    if (aInterfaceName == nullptr)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    if (!sBpfObj)
    {
        fprintf(stderr, "AttachToInterface: sBpfObj is NULL\n");
        return OT_ERROR_INVALID_STATE;
    }

    unsigned int ifindex = if_nametoindex(aInterfaceName);
    if (ifindex == 0)
    {
        fprintf(stderr, "AttachToInterface: if_nametoindex(%s) failed (errno=%d)\n", aInterfaceName, errno);
        return OT_ERROR_NOT_FOUND;
    }

    // 1. Update config_map with the thread_ifindex
    int configMapFd = bpf_object__find_map_fd_by_name(sBpfObj, "config_map");
    if (configMapFd >= 0)
    {
        uint32_t key   = 0;
        uint32_t value = ifindex;
        if (bpf_map_update_elem(configMapFd, &key, &value, BPF_ANY) < 0)
        {
            otbrLogWarning("BPF Filter: Failed to update config_map: %s", strerror(errno));
        }
        else
        {
            otbrLogInfo("BPF Filter: Set thread_ifindex to %d in config_map", ifindex);
        }
    }
    else
    {
        otbrLogWarning("BPF Filter: Failed to find config_map");
    }

    // 2. Find BPF program 'tc_filter'
    struct bpf_program *prog = bpf_object__find_program_by_name(sBpfObj, "tc_filter");
    if (!prog)
    {
        fprintf(stderr, "AttachToInterface: failed to find program 'tc_filter'\n");
        return OT_ERROR_NOT_FOUND;
    }

    int progFd = bpf_program__fd(prog);
    if (progFd < 0)
    {
        fprintf(stderr, "AttachToInterface: failed to get program FD\n");
        return OT_ERROR_FAILED;
    }

    // 3. Attach using TC API
    struct bpf_tc_hook hook;
    memset(&hook, 0, sizeof(hook));
    hook.sz           = sizeof(hook);
    hook.ifindex      = ifindex;
    hook.attach_point = BPF_TC_EGRESS;

    int err = bpf_tc_hook_create(&hook);
    if (err < 0 && err != -EEXIST)
    {
        fprintf(stderr, "bpf_tc_hook_create failed: %s (err=%d)\n", strerror(-err), err);
        return OT_ERROR_FAILED;
    }

    struct bpf_tc_opts opts;
    memset(&opts, 0, sizeof(opts));
    opts.sz       = sizeof(opts);
    opts.prog_fd  = progFd;
    opts.priority = kTcPriority;
    opts.handle   = kTcHandle;

    err = bpf_tc_attach(&hook, &opts);
    if (err == -EEXIST)
    {
        opts.flags = BPF_TC_F_REPLACE;
        err = bpf_tc_attach(&hook, &opts);
    }
    if (err < 0)
    {
        fprintf(stderr, "bpf_tc_attach failed: %s (err=%d)\n", strerror(-err), err);
        return OT_ERROR_FAILED;
    }

    sIsAttached      = true;
    sAttachedIfIndex = ifindex;

    otbrLogInfo("BPF Filter: Attached TC filter to %s (priority=%d, handle=%d)", aInterfaceName, kTcPriority,
                kTcHandle);
    return OT_ERROR_NONE;
}

otError DetachFromInterface(const char *aInterfaceName)
{
    if (aInterfaceName == nullptr)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    if (!sIsAttached)
    {
        return OT_ERROR_NONE;
    }

    unsigned int ifindex = if_nametoindex(aInterfaceName);
    if (ifindex != static_cast<unsigned int>(sAttachedIfIndex))
    {
        fprintf(stderr, "DetachFromInterface: interface %s (index %d) does not match attached index %d\n",
                aInterfaceName, ifindex, sAttachedIfIndex);
        return OT_ERROR_INVALID_ARGS;
    }

    struct bpf_tc_hook hook;
    memset(&hook, 0, sizeof(hook));
    hook.sz           = sizeof(hook);
    hook.ifindex      = sAttachedIfIndex;
    hook.attach_point = BPF_TC_EGRESS;

    struct bpf_tc_opts opts;
    memset(&opts, 0, sizeof(opts));
    opts.sz       = sizeof(opts);
    opts.priority = kTcPriority;
    opts.handle   = kTcHandle;
    opts.prog_id  = 0; // to detach

    int err = bpf_tc_detach(&hook, &opts);
    if (err < 0)
    {
        fprintf(stderr, "bpf_tc_detach failed: %s (err=%d)\n", strerror(-err), err);
        return OT_ERROR_FAILED;
    }

    sIsAttached      = false;
    sAttachedIfIndex = -1;
    otbrLogInfo("BPF Filter: Detached TC filter from %s", aInterfaceName);
    return OT_ERROR_NONE;
}

} // namespace bpf
} // namespace otbr
