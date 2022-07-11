#
#  Copyright (c) 2018, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

LOCAL_PATH := $(call my-dir)

ifeq ($(OTBR_ENABLE_ANDROID_MK),1)

OPENTHREAD_PATH := $(LOCAL_PATH)/../openthread

OPENTHREAD_DEFAULT_VERSION := $(shell cat $(OPENTHREAD_PATH)/.default-version)
OPENTHREAD_SOURCE_VERSION := $(shell git -C $(OPENTHREAD_PATH) describe --always --match "[0-9].*" 2> /dev/null)

OPENTHREAD_PROJECT_CFLAGS                                                 ?= \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"openthread-core-posix-config.h\" \
    $(NULL)

OPENTHREAD_PUBLIC_CFLAGS                                         := \
    -DOPENTHREAD_CONFIG_BORDER_AGENT_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_BORDER_ROUTER_ENABLE=1                      \
    -DOPENTHREAD_CONFIG_CHILD_SUPERVISION_ENABLE=1                  \
    -DOPENTHREAD_CONFIG_DTLS_ENABLE=1                               \
    -DOPENTHREAD_CONFIG_IP6_SLAAC_ENABLE=1                          \
    -DOPENTHREAD_CONFIG_JAM_DETECTION_ENABLE=1                      \
    -DOPENTHREAD_CONFIG_JOINER_ENABLE=1                             \
    -DOPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE=1                  \
    -DOPENTHREAD_CONFIG_MAC_FILTER_ENABLE=1                         \
    -DOPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1                           \
    -DOPENTHREAD_CONFIG_PING_SENDER_ENABLE=1                        \
    -DOPENTHREAD_CONFIG_TMF_NETDATA_SERVICE_ENABLE=1                \
    -DOPENTHREAD_FTD=1                                              \
    -DOPENTHREAD_PLATFORM_POSIX=1                                   \
    -DOPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE=1                      \
    -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1          \
    $(NULL)

OPENTHREAD_PRIVATE_CFLAGS                                        := \
    -DMBEDTLS_CONFIG_FILE=\"mbedtls-config.h\"                      \
    -DPACKAGE=\"openthread\"                                        \
    -DPACKAGE_BUGREPORT=\"openthread-devel@googlegroups.com\"       \
    -DPACKAGE_NAME=\"OPENTHREAD\"                                   \
    -DPACKAGE_STRING=\"OPENTHREAD\ $(OPENTHREAD_DEFAULT_VERSION)\"  \
    -DPACKAGE_TARNAME=\"openthread\"                                \
    -DPACKAGE_URL=\"http://github.com/openthread/openthread\"       \
    -DPACKAGE_VERSION=\"$(OPENTHREAD_SOURCE_VERSION)\"              \
    -DSPINEL_PLATFORM_HEADER=\"spinel_platform.h\"                  \
    -DVERSION=\"$(OPENTHREAD_DEFAULT_VERSION)\"                     \
    $(NULL)

# Enable required features for on-device tests.
ifeq ($(TARGET_BUILD_VARIANT),eng)
OPENTHREAD_PUBLIC_CFLAGS                                         += \
    -DOPENTHREAD_CONFIG_DIAG_ENABLE=1                               \
    $(NULL)
endif

ifeq ($(USE_OTBR_DAEMON), 1)
OPENTHREAD_PUBLIC_CFLAGS                                         += \
    -DOPENTHREAD_CONFIG_PLATFORM_NETIF_ENABLE=1                     \
    -DOPENTHREAD_CONFIG_PLATFORM_UDP_ENABLE=1                       \
    -DOPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=1                       \
    $(NULL)
else
OPENTHREAD_PUBLIC_CFLAGS += -DOPENTHREAD_CONFIG_UDP_FORWARD_ENABLE=1
endif

ifeq ($(USE_OT_RCP_BUS), spi)
OPENTHREAD_PUBLIC_CFLAGS += -DOPENTHREAD_POSIX_CONFIG_RCP_BUS=OT_POSIX_RCP_BUS_SPI
else
OPENTHREAD_PUBLIC_CFLAGS += -DOPENTHREAD_POSIX_CONFIG_RCP_BUS=OT_POSIX_RCP_BUS_UART
endif

# Enable all optional features for CI tests.
ifeq ($(TARGET_PRODUCT),generic)
OPENTHREAD_PUBLIC_CFLAGS                                         += \
    -DOPENTHREAD_CONFIG_COAP_API_ENABLE=1                           \
    -DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1                       \
    -DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1                         \
    -DOPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE=0                   \
    -DOPENTHREAD_CONFIG_TMF_NETWORK_DIAG_MTD_ENABLE=1               \
    $(NULL)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := spi-hdlc-adapter
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES := tools/spi-hdlc-adapter/spi-hdlc-adapter.c

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := ot-core
LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(OPENTHREAD_PATH)/include                                   \
    $(OPENTHREAD_PATH)/src                                       \
    $(OPENTHREAD_PATH)/src/cli                                   \
    $(OPENTHREAD_PATH)/src/core                                  \
    $(OPENTHREAD_PATH)/src/ncp                                   \
    $(OPENTHREAD_PATH)/third_party                               \
    $(OPENTHREAD_PATH)/third_party/mbedtls                       \
    $(OPENTHREAD_PATH)/third_party/mbedtls/repo/include          \
    $(OPENTHREAD_PATH)/third_party/mbedtls/repo/library          \
    $(LOCAL_PATH)/src                                            \
    $(LOCAL_PATH)/src/posix/platform                             \
    $(LOCAL_PATH)/src/posix/platform/include                     \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_EXPORT_CFLAGS                                        := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                              \
    $(NULL)

LOCAL_EXPORT_C_INCLUDE_DIRS     := \
    $(OPENTHREAD_PROJECT_INCLUDES) \
    $(OPENTHREAD_PATH)/include          \
    $(OPENTHREAD_PATH)/src              \
    $(NULL)

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -Wno-error=non-virtual-dtor \
    -pedantic-errors                                                           \
    $(NULL)

ifeq ($(ANDROID_NDK),1)
LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_CFLAGS                                             += \
    -DOPENTHREAD_ENABLE_ANDROID_NDK=1                       \
    -Wno-sign-compare                                       \
    $(NULL)
endif

LOCAL_SRC_FILES                                                                     := \
    src/posix/platform/alarm.cpp                                    \
    src/posix/platform/backbone.cpp                                 \
    src/posix/platform/daemon.cpp                                   \
    src/posix/platform/entropy.cpp                                  \
    src/posix/platform/firewall.cpp                                 \
    src/posix/platform/hdlc_interface.cpp                           \
    src/posix/platform/infra_if.cpp                                 \
    src/posix/platform/logging.cpp                                  \
    src/posix/platform/mainloop.cpp                                 \
    src/posix/platform/memory.cpp                                   \
    src/posix/platform/misc.cpp                                     \
    src/posix/platform/multicast_routing.cpp                        \
    src/posix/platform/netif.cpp                                    \
    src/posix/platform/radio.cpp                                    \
    src/posix/platform/radio_url.cpp                                \
    src/posix/platform/settings.cpp                                 \
    src/posix/platform/spi_interface.cpp                            \
    src/posix/platform/system.cpp                                   \
    src/posix/platform/trel.cpp                                     \
    src/posix/platform/udp.cpp                                      \
    src/posix/platform/utils.cpp                                    \
    ../openthread/src/core/api/backbone_router_api.cpp                            \
    ../openthread/src/core/api/backbone_router_ftd_api.cpp                        \
    ../openthread/src/core/api/border_agent_api.cpp                               \
    ../openthread/src/core/api/border_router_api.cpp                              \
    ../openthread/src/core/api/channel_manager_api.cpp                            \
    ../openthread/src/core/api/channel_monitor_api.cpp                            \
    ../openthread/src/core/api/child_supervision_api.cpp                          \
    ../openthread/src/core/api/coap_api.cpp                                       \
    ../openthread/src/core/api/coap_secure_api.cpp                                \
    ../openthread/src/core/api/commissioner_api.cpp                               \
    ../openthread/src/core/api/crypto_api.cpp                                     \
    ../openthread/src/core/api/dataset_api.cpp                                    \
    ../openthread/src/core/api/dataset_ftd_api.cpp                                \
    ../openthread/src/core/api/dataset_updater_api.cpp                            \
    ../openthread/src/core/api/diags_api.cpp                                      \
    ../openthread/src/core/api/dns_api.cpp                                        \
    ../openthread/src/core/api/dns_server_api.cpp                                 \
    ../openthread/src/core/api/error_api.cpp                                      \
    ../openthread/src/core/api/heap_api.cpp                                       \
    ../openthread/src/core/api/history_tracker_api.cpp                            \
    ../openthread/src/core/api/icmp6_api.cpp                                      \
    ../openthread/src/core/api/instance_api.cpp                                   \
    ../openthread/src/core/api/ip6_api.cpp                                        \
    ../openthread/src/core/api/jam_detection_api.cpp                              \
    ../openthread/src/core/api/joiner_api.cpp                                     \
    ../openthread/src/core/api/link_api.cpp                                       \
    ../openthread/src/core/api/link_metrics_api.cpp                               \
    ../openthread/src/core/api/link_raw_api.cpp                                   \
    ../openthread/src/core/api/logging_api.cpp                                    \
    ../openthread/src/core/api/message_api.cpp                                    \
    ../openthread/src/core/api/multi_radio_api.cpp                                \
    ../openthread/src/core/api/netdata_api.cpp                                    \
    ../openthread/src/core/api/netdata_publisher_api.cpp                          \
    ../openthread/src/core/api/netdiag_api.cpp                                    \
    ../openthread/src/core/api/network_time_api.cpp                               \
    ../openthread/src/core/api/ping_sender_api.cpp                                \
    ../openthread/src/core/api/random_crypto_api.cpp                              \
    ../openthread/src/core/api/random_noncrypto_api.cpp                           \
    ../openthread/src/core/api/server_api.cpp                                     \
    ../openthread/src/core/api/sntp_api.cpp                                       \
    ../openthread/src/core/api/srp_client_api.cpp                                 \
    ../openthread/src/core/api/srp_client_buffers_api.cpp                         \
    ../openthread/src/core/api/srp_server_api.cpp                                 \
    ../openthread/src/core/api/tasklet_api.cpp                                    \
    ../openthread/src/core/api/tcp_api.cpp                                        \
    ../openthread/src/core/api/thread_api.cpp                                     \
    ../openthread/src/core/api/thread_ftd_api.cpp                                 \
    ../openthread/src/core/api/trel_api.cpp                                       \
    ../openthread/src/core/api/udp_api.cpp                                        \
    ../openthread/src/core/backbone_router/backbone_tmf.cpp                       \
    ../openthread/src/core/backbone_router/bbr_leader.cpp                         \
    ../openthread/src/core/backbone_router/bbr_local.cpp                          \
    ../openthread/src/core/backbone_router/bbr_manager.cpp                        \
    ../openthread/src/core/backbone_router/multicast_listeners_table.cpp          \
    ../openthread/src/core/backbone_router/ndproxy_table.cpp                      \
    ../openthread/src/core/border_router/infra_if.cpp                             \
    ../openthread/src/core/border_router/routing_manager.cpp                      \
    ../openthread/src/core/coap/coap.cpp                                          \
    ../openthread/src/core/coap/coap_message.cpp                                  \
    ../openthread/src/core/coap/coap_secure.cpp                                   \
    ../openthread/src/core/common/appender.cpp                                    \
    ../openthread/src/core/common/binary_search.cpp                               \
    ../openthread/src/core/common/crc16.cpp                                       \
    ../openthread/src/core/common/data.cpp                                        \
    ../openthread/src/core/common/error.cpp                                       \
    ../openthread/src/core/common/heap.cpp                                        \
    ../openthread/src/core/common/heap_data.cpp                                   \
    ../openthread/src/core/common/heap_string.cpp                                 \
    ../openthread/src/core/common/instance.cpp                                    \
    ../openthread/src/core/common/log.cpp                                         \
    ../openthread/src/core/common/message.cpp                                     \
    ../openthread/src/core/common/notifier.cpp                                    \
    ../openthread/src/core/common/random.cpp                                      \
    ../openthread/src/core/common/settings.cpp                                    \
    ../openthread/src/core/common/string.cpp                                      \
    ../openthread/src/core/common/tasklet.cpp                                     \
    ../openthread/src/core/common/time_ticker.cpp                                 \
    ../openthread/src/core/common/timer.cpp                                       \
    ../openthread/src/core/common/tlvs.cpp                                        \
    ../openthread/src/core/common/trickle_timer.cpp                               \
    ../openthread/src/core/common/uptime.cpp                                      \
    ../openthread/src/core/crypto/aes_ccm.cpp                                     \
    ../openthread/src/core/crypto/aes_ecb.cpp                                     \
    ../openthread/src/core/crypto/crypto_platform.cpp                             \
    ../openthread/src/core/crypto/ecdsa.cpp                                       \
    ../openthread/src/core/crypto/ecdsa_tinycrypt.cpp                             \
    ../openthread/src/core/crypto/hkdf_sha256.cpp                                 \
    ../openthread/src/core/crypto/hmac_sha256.cpp                                 \
    ../openthread/src/core/crypto/mbedtls.cpp                                     \
    ../openthread/src/core/crypto/pbkdf2_cmac.cpp                                 \
    ../openthread/src/core/crypto/sha256.cpp                                      \
    ../openthread/src/core/crypto/storage.cpp                                     \
    ../openthread/src/core/diags/factory_diags.cpp                                \
    ../openthread/src/core/mac/channel_mask.cpp                                   \
    ../openthread/src/core/mac/data_poll_handler.cpp                              \
    ../openthread/src/core/mac/data_poll_sender.cpp                               \
    ../openthread/src/core/mac/link_raw.cpp                                       \
    ../openthread/src/core/mac/mac.cpp                                            \
    ../openthread/src/core/mac/mac_filter.cpp                                     \
    ../openthread/src/core/mac/mac_frame.cpp                                      \
    ../openthread/src/core/mac/mac_links.cpp                                      \
    ../openthread/src/core/mac/mac_types.cpp                                      \
    ../openthread/src/core/mac/sub_mac.cpp                                        \
    ../openthread/src/core/mac/sub_mac_callbacks.cpp                              \
    ../openthread/src/core/meshcop/announce_begin_client.cpp                      \
    ../openthread/src/core/meshcop/border_agent.cpp                               \
    ../openthread/src/core/meshcop/commissioner.cpp                               \
    ../openthread/src/core/meshcop/dataset.cpp                                    \
    ../openthread/src/core/meshcop/dataset_local.cpp                              \
    ../openthread/src/core/meshcop/dataset_manager.cpp                            \
    ../openthread/src/core/meshcop/dataset_manager_ftd.cpp                        \
    ../openthread/src/core/meshcop/dataset_updater.cpp                            \
    ../openthread/src/core/meshcop/dtls.cpp                                       \
    ../openthread/src/core/meshcop/energy_scan_client.cpp                         \
    ../openthread/src/core/meshcop/extended_panid.cpp                             \
    ../openthread/src/core/meshcop/joiner.cpp                                     \
    ../openthread/src/core/meshcop/joiner_router.cpp                              \
    ../openthread/src/core/meshcop/meshcop.cpp                                    \
    ../openthread/src/core/meshcop/meshcop_leader.cpp                             \
    ../openthread/src/core/meshcop/meshcop_tlvs.cpp                               \
    ../openthread/src/core/meshcop/network_name.cpp                               \
    ../openthread/src/core/meshcop/panid_query_client.cpp                         \
    ../openthread/src/core/meshcop/timestamp.cpp                                  \
    ../openthread/src/core/net/checksum.cpp                                       \
    ../openthread/src/core/net/dhcp6_client.cpp                                   \
    ../openthread/src/core/net/dhcp6_server.cpp                                   \
    ../openthread/src/core/net/dns_client.cpp                                     \
    ../openthread/src/core/net/dns_dso.cpp                                        \
    ../openthread/src/core/net/dns_types.cpp                                      \
    ../openthread/src/core/net/dnssd_server.cpp                                   \
    ../openthread/src/core/net/icmp6.cpp                                          \
    ../openthread/src/core/net/ip4_address.cpp                                    \
    ../openthread/src/core/net/ip6.cpp                                            \
    ../openthread/src/core/net/ip6_address.cpp                                    \
    ../openthread/src/core/net/ip6_filter.cpp                                     \
    ../openthread/src/core/net/ip6_headers.cpp                                    \
    ../openthread/src/core/net/ip6_mpl.cpp                                        \
    ../openthread/src/core/net/nd6.cpp                                            \
    ../openthread/src/core/net/nd_agent.cpp                                       \
    ../openthread/src/core/net/netif.cpp                                          \
    ../openthread/src/core/net/sntp_client.cpp                                    \
    ../openthread/src/core/net/socket.cpp                                         \
    ../openthread/src/core/net/srp_client.cpp                                     \
    ../openthread/src/core/net/srp_server.cpp                                     \
    ../openthread/src/core/net/tcp6.cpp                                           \
    ../openthread/src/core/net/udp6.cpp                                           \
    ../openthread/src/core/radio/radio.cpp                                        \
    ../openthread/src/core/radio/radio_callbacks.cpp                              \
    ../openthread/src/core/radio/radio_platform.cpp                               \
    ../openthread/src/core/radio/trel_interface.cpp                               \
    ../openthread/src/core/radio/trel_link.cpp                                    \
    ../openthread/src/core/radio/trel_packet.cpp                                  \
    ../openthread/src/core/thread/address_resolver.cpp                            \
    ../openthread/src/core/thread/announce_begin_server.cpp                       \
    ../openthread/src/core/thread/announce_sender.cpp                             \
    ../openthread/src/core/thread/anycast_locator.cpp                             \
    ../openthread/src/core/thread/child_table.cpp                                 \
    ../openthread/src/core/thread/csl_tx_scheduler.cpp                            \
    ../openthread/src/core/thread/discover_scanner.cpp                            \
    ../openthread/src/core/thread/dua_manager.cpp                                 \
    ../openthread/src/core/thread/energy_scan_server.cpp                          \
    ../openthread/src/core/thread/indirect_sender.cpp                             \
    ../openthread/src/core/thread/key_manager.cpp                                 \
    ../openthread/src/core/thread/link_metrics.cpp                                \
    ../openthread/src/core/thread/link_quality.cpp                                \
    ../openthread/src/core/thread/lowpan.cpp                                      \
    ../openthread/src/core/thread/mesh_forwarder.cpp                              \
    ../openthread/src/core/thread/mesh_forwarder_ftd.cpp                          \
    ../openthread/src/core/thread/mesh_forwarder_mtd.cpp                          \
    ../openthread/src/core/thread/mle.cpp                                         \
    ../openthread/src/core/thread/mle_router.cpp                                  \
    ../openthread/src/core/thread/mle_types.cpp                                   \
    ../openthread/src/core/thread/mlr_manager.cpp                                 \
    ../openthread/src/core/thread/neighbor_table.cpp                              \
    ../openthread/src/core/thread/network_data.cpp                                \
    ../openthread/src/core/thread/network_data_leader.cpp                         \
    ../openthread/src/core/thread/network_data_leader_ftd.cpp                     \
    ../openthread/src/core/thread/network_data_local.cpp                          \
    ../openthread/src/core/thread/network_data_notifier.cpp                       \
    ../openthread/src/core/thread/network_data_publisher.cpp                      \
    ../openthread/src/core/thread/network_data_service.cpp                        \
    ../openthread/src/core/thread/network_data_tlvs.cpp                           \
    ../openthread/src/core/thread/network_data_types.cpp                          \
    ../openthread/src/core/thread/network_diagnostic.cpp                          \
    ../openthread/src/core/thread/panid_query_server.cpp                          \
    ../openthread/src/core/thread/radio_selector.cpp                              \
    ../openthread/src/core/thread/router_table.cpp                                \
    ../openthread/src/core/thread/src_match_controller.cpp                        \
    ../openthread/src/core/thread/thread_netif.cpp                                \
    ../openthread/src/core/thread/time_sync_service.cpp                           \
    ../openthread/src/core/thread/tmf.cpp                                         \
    ../openthread/src/core/thread/topology.cpp                                    \
    ../openthread/src/core/thread/uri_paths.cpp                                   \
    ../openthread/src/core/utils/channel_manager.cpp                              \
    ../openthread/src/core/utils/channel_monitor.cpp                              \
    ../openthread/src/core/utils/child_supervision.cpp                            \
    ../openthread/src/core/utils/flash.cpp                                        \
    ../openthread/src/core/utils/heap.cpp                                         \
    ../openthread/src/core/utils/history_tracker.cpp                              \
    ../openthread/src/core/utils/jam_detector.cpp                                 \
    ../openthread/src/core/utils/otns.cpp                                         \
    ../openthread/src/core/utils/parse_cmdline.cpp                                \
    ../openthread/src/core/utils/ping_sender.cpp                                  \
    ../openthread/src/core/utils/slaac_address.cpp                                \
    ../openthread/src/core/utils/srp_client_buffers.cpp                           \
    ../openthread/src/lib/hdlc/hdlc.cpp                                           \
    ../openthread/src/lib/platform/exit_code.c                                    \
    ../openthread/src/lib/spinel/spinel.c                                         \
    ../openthread/src/lib/spinel/spinel_decoder.cpp                               \
    ../openthread/src/lib/spinel/spinel_encoder.cpp                               \
    ../openthread/src/lib/url/url.cpp                                             \
    ../openthread/third_party/mbedtls/repo/library/aes.c                          \
    ../openthread/third_party/mbedtls/repo/library/aesni.c                        \
    ../openthread/third_party/mbedtls/repo/library/arc4.c                         \
    ../openthread/third_party/mbedtls/repo/library/aria.c                         \
    ../openthread/third_party/mbedtls/repo/library/asn1parse.c                    \
    ../openthread/third_party/mbedtls/repo/library/asn1write.c                    \
    ../openthread/third_party/mbedtls/repo/library/base64.c                       \
    ../openthread/third_party/mbedtls/repo/library/bignum.c                       \
    ../openthread/third_party/mbedtls/repo/library/blowfish.c                     \
    ../openthread/third_party/mbedtls/repo/library/camellia.c                     \
    ../openthread/third_party/mbedtls/repo/library/ccm.c                          \
    ../openthread/third_party/mbedtls/repo/library/certs.c                        \
    ../openthread/third_party/mbedtls/repo/library/chacha20.c                     \
    ../openthread/third_party/mbedtls/repo/library/chachapoly.c                   \
    ../openthread/third_party/mbedtls/repo/library/cipher.c                       \
    ../openthread/third_party/mbedtls/repo/library/cipher_wrap.c                  \
    ../openthread/third_party/mbedtls/repo/library/cmac.c                         \
    ../openthread/third_party/mbedtls/repo/library/constant_time.c                \
    ../openthread/third_party/mbedtls/repo/library/ctr_drbg.c                     \
    ../openthread/third_party/mbedtls/repo/library/debug.c                        \
    ../openthread/third_party/mbedtls/repo/library/des.c                          \
    ../openthread/third_party/mbedtls/repo/library/dhm.c                          \
    ../openthread/third_party/mbedtls/repo/library/ecdh.c                         \
    ../openthread/third_party/mbedtls/repo/library/ecdsa.c                        \
    ../openthread/third_party/mbedtls/repo/library/ecjpake.c                      \
    ../openthread/third_party/mbedtls/repo/library/ecp.c                          \
    ../openthread/third_party/mbedtls/repo/library/ecp_curves.c                   \
    ../openthread/third_party/mbedtls/repo/library/entropy.c                      \
    ../openthread/third_party/mbedtls/repo/library/entropy_poll.c                 \
    ../openthread/third_party/mbedtls/repo/library/error.c                        \
    ../openthread/third_party/mbedtls/repo/library/gcm.c                          \
    ../openthread/third_party/mbedtls/repo/library/havege.c                       \
    ../openthread/third_party/mbedtls/repo/library/hkdf.c                         \
    ../openthread/third_party/mbedtls/repo/library/hmac_drbg.c                    \
    ../openthread/third_party/mbedtls/repo/library/md.c                           \
    ../openthread/third_party/mbedtls/repo/library/md2.c                          \
    ../openthread/third_party/mbedtls/repo/library/md4.c                          \
    ../openthread/third_party/mbedtls/repo/library/md5.c                          \
    ../openthread/third_party/mbedtls/repo/library/memory_buffer_alloc.c          \
    ../openthread/third_party/mbedtls/repo/library/net_sockets.c                  \
    ../openthread/third_party/mbedtls/repo/library/nist_kw.c                      \
    ../openthread/third_party/mbedtls/repo/library/oid.c                          \
    ../openthread/third_party/mbedtls/repo/library/padlock.c                      \
    ../openthread/third_party/mbedtls/repo/library/pem.c                          \
    ../openthread/third_party/mbedtls/repo/library/pk.c                           \
    ../openthread/third_party/mbedtls/repo/library/pk_wrap.c                      \
    ../openthread/third_party/mbedtls/repo/library/pkcs11.c                       \
    ../openthread/third_party/mbedtls/repo/library/pkcs12.c                       \
    ../openthread/third_party/mbedtls/repo/library/pkcs5.c                        \
    ../openthread/third_party/mbedtls/repo/library/pkparse.c                      \
    ../openthread/third_party/mbedtls/repo/library/pkwrite.c                      \
    ../openthread/third_party/mbedtls/repo/library/platform.c                     \
    ../openthread/third_party/mbedtls/repo/library/platform_util.c                \
    ../openthread/third_party/mbedtls/repo/library/poly1305.c                     \
    ../openthread/third_party/mbedtls/repo/library/psa_crypto.c                   \
    ../openthread/third_party/mbedtls/repo/library/psa_crypto_driver_wrappers.c   \
    ../openthread/third_party/mbedtls/repo/library/psa_crypto_se.c                \
    ../openthread/third_party/mbedtls/repo/library/psa_crypto_slot_management.c   \
    ../openthread/third_party/mbedtls/repo/library/psa_crypto_storage.c           \
    ../openthread/third_party/mbedtls/repo/library/psa_its_file.c                 \
    ../openthread/third_party/mbedtls/repo/library/ripemd160.c                    \
    ../openthread/third_party/mbedtls/repo/library/rsa.c                          \
    ../openthread/third_party/mbedtls/repo/library/rsa_internal.c                 \
    ../openthread/third_party/mbedtls/repo/library/sha1.c                         \
    ../openthread/third_party/mbedtls/repo/library/sha256.c                       \
    ../openthread/third_party/mbedtls/repo/library/sha512.c                       \
    ../openthread/third_party/mbedtls/repo/library/ssl_cache.c                    \
    ../openthread/third_party/mbedtls/repo/library/ssl_ciphersuites.c             \
    ../openthread/third_party/mbedtls/repo/library/ssl_cli.c                      \
    ../openthread/third_party/mbedtls/repo/library/ssl_cookie.c                   \
    ../openthread/third_party/mbedtls/repo/library/ssl_msg.c                      \
    ../openthread/third_party/mbedtls/repo/library/ssl_srv.c                      \
    ../openthread/third_party/mbedtls/repo/library/ssl_ticket.c                   \
    ../openthread/third_party/mbedtls/repo/library/ssl_tls.c                      \
    ../openthread/third_party/mbedtls/repo/library/ssl_tls13_keys.c               \
    ../openthread/third_party/mbedtls/repo/library/threading.c                    \
    ../openthread/third_party/mbedtls/repo/library/timing.c                       \
    ../openthread/third_party/mbedtls/repo/library/version.c                      \
    ../openthread/third_party/mbedtls/repo/library/version_features.c             \
    ../openthread/third_party/mbedtls/repo/library/x509.c                         \
    ../openthread/third_party/mbedtls/repo/library/x509_create.c                  \
    ../openthread/third_party/mbedtls/repo/library/x509_crl.c                     \
    ../openthread/third_party/mbedtls/repo/library/x509_crt.c                     \
    ../openthread/third_party/mbedtls/repo/library/x509_csr.c                     \
    ../openthread/third_party/mbedtls/repo/library/x509write_crt.c                \
    ../openthread/third_party/mbedtls/repo/library/x509write_csr.c                \
    ../openthread/third_party/mbedtls/repo/library/xtea.c                         \
    ../openthread/third_party/tcplp/bsdtcp/tcp_usrreq.c                           \
    ../openthread/third_party/tcplp/bsdtcp/tcp_subr.c                             \
    ../openthread/third_party/tcplp/bsdtcp/tcp_output.c                           \
    ../openthread/third_party/tcplp/bsdtcp/cc/cc_newreno.c                        \
    ../openthread/third_party/tcplp/bsdtcp/tcp_reass.c                            \
    ../openthread/third_party/tcplp/bsdtcp/tcp_timewait.c                         \
    ../openthread/third_party/tcplp/bsdtcp/tcp_sack.c                             \
    ../openthread/third_party/tcplp/bsdtcp/tcp_input.c                            \
    ../openthread/third_party/tcplp/bsdtcp/tcp_timer.c                            \
    ../openthread/third_party/tcplp/lib/bitmap.c                                  \
    ../openthread/third_party/tcplp/lib/cbuf.c                                    \
    ../openthread/third_party/tcplp/lib/lbuf.c                                    \
    $(OPENTHREAD_PROJECT_SRC_FILES)                                 \
    $(NULL)

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libopenthread-cli
LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(OPENTHREAD_PATH)/include                                   \
    $(OPENTHREAD_PATH)/src                                       \
    $(OPENTHREAD_PATH)/src/cli                                   \
    $(OPENTHREAD_PATH)/src/core                                  \
    $(LOCAL_PATH)/src/posix/platform                             \
    $(LOCAL_PATH)/src/posix/platform/include                     \
    $(OPENTHREAD_PATH)/third_party/mbedtls                       \
    $(OPENTHREAD_PATH)/third_party/mbedtls/repo/include          \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -pedantic-errors                                                           \
    $(NULL)

LOCAL_SRC_FILES                                               := \
    ../openthread/src/cli/cli.cpp                           \
    ../openthread/src/cli/cli_coap.cpp                      \
    ../openthread/src/cli/cli_coap_secure.cpp               \
    ../openthread/src/cli/cli_commissioner.cpp              \
    ../openthread/src/cli/cli_dataset.cpp                   \
    ../openthread/src/cli/cli_history.cpp                   \
    ../openthread/src/cli/cli_joiner.cpp                    \
    ../openthread/src/cli/cli_network_data.cpp              \
    ../openthread/src/cli/cli_output.cpp                    \
    ../openthread/src/cli/cli_srp_client.cpp                \
    ../openthread/src/cli/cli_srp_server.cpp                \
    ../openthread/src/cli/cli_tcp.cpp                       \
    ../openthread/src/cli/cli_udp.cpp                       \
    $(NULL)

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := ot-cli
LOCAL_MODULE_TAGS := eng

ifneq ($(ANDROID_NDK),1)
LOCAL_SHARED_LIBRARIES := libcutils
endif

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(OPENTHREAD_PATH)/include                                   \
    $(OPENTHREAD_PATH)/src                                       \
    $(OPENTHREAD_PATH)/src/cli                                   \
    $(OPENTHREAD_PATH)/src/core                                  \
    $(LOCAL_PATH)/src/posix/platform                             \
    $(LOCAL_PATH)/src/posix/platform/include                     \
    $(OPENTHREAD_PATH)/third_party/mbedtls                       \
    $(OPENTHREAD_PATH)/third_party/mbedtls/repo/include          \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -pedantic-errors                                                           \
    $(NULL)

LOCAL_LDLIBS                               := \
    -lrt                                      \
    -lutil

LOCAL_SRC_FILES                                               := \
    ../openthread/src/posix/cli_readline.cpp                \
    ../openthread/src/posix/cli_stdio.cpp                   \
    ../openthread/src/posix/main.c                          \
    $(NULL)

LOCAL_STATIC_LIBRARIES = libopenthread-cli ot-core
include $(BUILD_EXECUTABLE)

ifeq ($(USE_OTBR_DAEMON), 1)
include $(CLEAR_VARS)

LOCAL_MODULE := ot-ctl
LOCAL_MODULE_TAGS := eng

LOCAL_CPPFLAGS                                                              := \
    -std=c++11                                                                 \
    -pedantic-errors                                                           \
    $(NULL)

LOCAL_CFLAGS                                                                := \
    $(OPENTHREAD_PUBLIC_CFLAGS)                                                \
    $(OPENTHREAD_PRIVATE_CFLAGS)                                               \
    $(OPENTHREAD_PROJECT_CFLAGS)                                               \
    $(NULL)

LOCAL_C_INCLUDES                                         := \
    $(OPENTHREAD_PROJECT_INCLUDES)                          \
    $(OPENTHREAD_PATH)/include                              \
    $(OPENTHREAD_PATH)/src/                                 \
    $(OPENTHREAD_PATH)/src/core                             \
    $(LOCAL_PATH)/src/posix/platform                        \
    $(LOCAL_PATH)/src/posix/platform/include                \
    $(NULL)

LOCAL_SRC_FILES := src/posix/client.cpp

include $(BUILD_EXECUTABLE)
endif # ($(USE_OTBR_DAEMON), 1)

ifneq ($(OPENTHREAD_PROJECT_ANDROID_MK),)
include $(OPENTHREAD_PROJECT_ANDROID_MK)
endif

ifneq ($(OTBR_PROJECT_ANDROID_MK),)
include $(OTBR_PROJECT_ANDROID_MK)
endif

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE := libotbr-dbus-client
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += -Wall -Wextra -Wno-unused-parameter

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/src \
    external/openthread/include \

LOCAL_SRC_FILES = \
    src/dbus/client/client_error.cpp \
    src/dbus/client/thread_api_dbus.cpp \
    src/dbus/common/dbus_message_helper.cpp \
    src/dbus/common/dbus_message_helper_openthread.cpp \
    src/dbus/common/error.cpp \

LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/src \
    $(LOCAL_PATH)/include \

LOCAL_SHARED_LIBRARIES += libdbus

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE := otbr-agent
LOCAL_MODULE_TAGS := eng
LOCAL_SHARED_LIBRARIES := libdbus

OTBR_GEN_HEADER_DIR := $(local-intermediates-dir)/gen
OTBR_GEN_DBUS_INTROSPECT_HEADER := $(OTBR_GEN_HEADER_DIR)/dbus/server/introspect.hpp

$(OTBR_GEN_DBUS_INTROSPECT_HEADER): $(LOCAL_PATH)/src/dbus/server/introspect.xml
	mkdir -p $(OTBR_GEN_HEADER_DIR)/dbus/server
	echo 'R"INTROSPECT(' > $@
	cat $+ >> $@
	echo ')INTROSPECT"' >> $@

$(LOCAL_PATH)/src/dbus/server/dbus_thread_object.cpp: $(OTBR_GEN_HEADER_DIR)/dbus/server/introspect.hpp

ifneq ($(ANDROID_NDK),1)
LOCAL_SHARED_LIBRARIES += libcutils
endif

OTBR_FEATURE_FLAGS_INCLUDES := $(local-intermediates-dir)/proto/$(LOCAL_PATH)/src

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/src \
    $(LOCAL_PATH)/src/posix/platform \
    $(LOCAL_PATH)/src/posix/platform/include \
    $(OPENTHREAD_PATH)/src \
    $(OPENTHREAD_PATH)/src/core \
    $(OPENTHREAD_PATH)/include \
    external/libchrome \
    external/gtest/include \
    external/openthread/include \
    external/openthread/src \
    $(OTBR_GEN_HEADER_DIR) \
    $(OTBR_PROJECT_INCLUDES) \
    $(OTBR_FEATURE_FLAGS_INCLUDES)

LOCAL_CFLAGS += -Wall -Wextra -Wno-unused-parameter
LOCAL_CFLAGS += \
    -DOTBR_PACKAGE_VERSION=\"0.2.0\" \
    -DOTBR_ENABLE_DBUS_SERVER=1 \
    -DOTBR_ENABLE_FEATURE_FLAGS=1 \
    -DOTBR_ENABLE_BORDER_AGENT=1 \
    -DOTBR_DBUS_INTROSPECT_FILE=\"\" \
    $(OTBR_PROJECT_CFLAGS) \

LOCAL_CPPFLAGS += -std=c++14

LOCAL_GENERATED_SOURCES = $(OTBR_GEN_DBUS_INTROSPECT_HEADER)

LOCAL_SRC_FILES := \
    src/agent/application.cpp \
    src/agent/main.cpp \
    src/backbone_router/backbone_agent.cpp \
    src/backbone_router/dua_routing_manager.cpp \
    src/backbone_router/nd_proxy.cpp \
    src/border_agent/border_agent.cpp \
    src/common/dns_utils.cpp \
    src/common/logging.cpp \
    src/common/mainloop.cpp \
    src/common/mainloop_manager.cpp \
    src/common/task_runner.cpp \
    src/common/types.cpp \
    src/dbus/common/dbus_message_dump.cpp \
    src/dbus/common/dbus_message_helper.cpp \
    src/dbus/common/dbus_message_helper_openthread.cpp \
    src/dbus/common/error.cpp \
    src/dbus/server/dbus_agent.cpp \
    src/dbus/server/dbus_object.cpp \
    src/dbus/server/dbus_thread_object.cpp \
    src/dbus/server/error_helper.cpp \
    src/mdns/mdns.cpp \
    src/ncp/ncp_openthread.cpp \
    src/proto/feature_flag.proto \
    src/sdp_proxy/advertising_proxy.cpp \
    src/sdp_proxy/discovery_proxy.cpp \
    src/trel_dnssd/trel_dnssd.cpp \
    src/utils/dns_utils.cpp \
    src/utils/hex.cpp \
    src/utils/infra_link_selector.cpp \
    src/utils/socket_utils.cpp \
    src/utils/string_utils.cpp \
    src/utils/thread_helper.cpp \

LOCAL_STATIC_LIBRARIES += \
    ot-core \
    libopenthread-cli \
    ot-core \
    libopenthread-mbedtls \

LOCAL_LDLIBS := \
    -lanl \
    -lutil

ifeq ($(OTBR_MDNS),mDNSResponder)
LOCAL_CFLAGS += \
    -DOTBR_ENABLE_MDNS_MDNSSD=1 \

LOCAL_SRC_FILES += \
    src/mdns/mdns_mdnssd.cpp \

LOCAL_SHARED_LIBRARIES += libmdnssd
endif

LOCAL_SRC_FILES += $(OTBR_PROJECT_SRC_FILES)
LOCAL_STATIC_LIBRARIES += $(OTBR_PROJECT_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES += $(OTBR_PROJECT_SHARED_LIBRARIES)

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE := otbr-agent.conf
LOCAL_MODULE_TAGS := eng

OTBR_AGENT_USER ?= root
OTBR_AGENT_GROUP ?= root

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/dbus-1/system.d
OTBR_GEN_DBUS_CONF_DIR := $(local-intermediates-dir)/gen
$(OTBR_GEN_DBUS_CONF_DIR)/otbr-agent.conf: $(LOCAL_PATH)/src/agent/otbr-agent.conf.in
	mkdir -p $(OTBR_GEN_DBUS_CONF_DIR)
	sed -e 's/@OTBR_AGENT_USER@/$(OTBR_AGENT_USER)/g' -e 's/@OTBR_AGENT_GROUP@/$(OTBR_AGENT_GROUP)/g' $< > $@

# Dirty hack for Android.mk to copy config files from the intermediate directory.
LOCAL_PATH := $(local-intermediates-dir)
LOCAL_SRC_FILES := gen/otbr-agent.conf

include $(BUILD_PREBUILT)
endif # ifeq ($(OTBR_ENABLE_ANDROID_MK),1)
