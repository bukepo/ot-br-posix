#include <linux/types.h>
#include <bpf/bpf_helpers.h>
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/pkt_cls.h>

#define bpf_htons(x) __builtin_bswap16(x)

struct otbr_lpm_key
{
    __u32           prefixlen;
    struct in6_addr prefix;
};

// Map containing Thread prefixes (OMR + ML)
// Used for blocked_src and to identify Thread destination
struct
{
    __uint(type, BPF_MAP_TYPE_LPM_TRIE);
    __uint(max_entries, 64);
    __type(key, struct otbr_lpm_key);
    __type(value, __u32);
    __uint(map_flags, BPF_F_NO_PREALLOC);
} thread_prefixes SEC(".maps");

// Map containing allowed destinations (OMR + DUA)
struct
{
    __uint(type, BPF_MAP_TYPE_LPM_TRIE);
    __uint(max_entries, 64);
    __type(key, struct otbr_lpm_key);
    __type(value, __u32);
    __uint(map_flags, BPF_F_NO_PREALLOC);
} allowed_dst SEC(".maps");

// Map for statistics: 0: PASS, 1: DROP
struct
{
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 2);
    __type(key, __u32);
    __type(value, __u64);
} filter_stats SEC(".maps");

// Map to store configuration (thread_ifindex)
struct
{
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, __u32);
} config_map SEC(".maps");

static __always_inline void count_action(__u32 action)
{
    __u32  map_key = (action == TC_ACT_OK) ? 0 : 1;
    __u64 *count   = bpf_map_lookup_elem(&filter_stats, &map_key);
    if (count)
    {
        __sync_fetch_and_add(count, 1);
    }
}

SEC("classifier")
int tc_filter(struct __sk_buff *skb)
{
    void               *data_end = (void *)(long)skb->data_end;
    void               *data     = (void *)(long)skb->data;
    struct ipv6hdr     *ip6      = data;
    struct otbr_lpm_key key;
    __u32              *value;

    if (skb->protocol != bpf_htons(ETH_P_IPV6))
    {
        return TC_ACT_OK;
    }

    if ((void *)(ip6 + 1) > data_end)
    {
        return TC_ACT_OK;
    }

    // 1. Bypass local traffic
    if (skb->ingress_ifindex == 0)
    {
        return TC_ACT_OK;
    }

    // 2. Drop Thread-to-Thread unicast traffic
    __u32  key_zero       = 0;
    __u32 *p_ifindex      = bpf_map_lookup_elem(&config_map, &key_zero);
    __u32  thread_ifindex = p_ifindex ? *p_ifindex : 0;

    if (thread_ifindex > 0 && skb->ingress_ifindex == thread_ifindex)
    {
        count_action(TC_ACT_SHOT);
        return TC_ACT_SHOT;
    }

    // 3. Block source if it matches Thread prefixes (OMR/ML) - Anti-spoofing
    key.prefixlen = 128;
    key.prefix    = ip6->saddr;
    value         = bpf_map_lookup_elem(&thread_prefixes, &key);
    if (value)
    {
        count_action(TC_ACT_SHOT);
        return TC_ACT_SHOT;
    }

    // 4. Allow multicast
    if (ip6->daddr.s6_addr[0] == 0xff)
    {
        count_action(TC_ACT_OK);
        return TC_ACT_OK;
    }

    // 5. Allow destination if it matches allowed destinations (OMR)
    key.prefixlen = 128;
    key.prefix    = ip6->daddr;
    value         = bpf_map_lookup_elem(&allowed_dst, &key);
    if (value)
    {
        count_action(TC_ACT_OK);
        return TC_ACT_OK;
    }

    // Default drop for other unicast
    count_action(TC_ACT_SHOT);
    return TC_ACT_SHOT;
}

char LICENSE[] SEC("license") = "GPL";
