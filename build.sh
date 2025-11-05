#!/bin/bash
[[ -f third_party/openthread/repo/CMakeLists.txt ]] || sudo mount -o bind ../../openthread/repo third_party/openthread/repo
./script/cmake-build -DOTBR_MDNS=avahi -DOT_LOG_LEVEL=INFO -DOT_TREL=OFF -DOTBR_BORDER_ROUTING=ON -DOT_FIREWALL=OFF -DOTBR_BORDER_ROUTER=ON -DOTBR_SRP_ADVERTISING_PROXY=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF "$@"
