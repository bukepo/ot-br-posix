#pragma once
#include <openthread/instance.h>
#include "openthread/error.h"

namespace otbr {
namespace bpf {

otError InitializeFilter(otInstance *aInstance);
void DeinitializeFilter(void);
otError UpdateFilterPrefixes(void);
otError AttachToInterface(const char *aInterfaceName);
otError DetachFromInterface(const char *aInterfaceName);

} // namespace bpf
} // namespace otbr
