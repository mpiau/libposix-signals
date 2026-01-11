#pragma once

#include "posix_signal_dispositions.h"
#include "posix_signal_enum.h"


//================================================================================================
// POSIX Signals Library
//================================================================================================

[[nodiscard]] bool psignal_library_init(void);
[[nodiscard]] bool psignal_library_is_running(void);
void psignal_library_shutdown(void);

[[nodiscard]] char const *psignal_library_description(void);

