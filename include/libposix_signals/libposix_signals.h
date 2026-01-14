#pragma once

#include "posix_signal_dispositions.h"
#include "posix_signal_enum.h"
#include "posix_signal_types.h"


//================================================================================================
// POSIX Signals Library
//================================================================================================

[[nodiscard]] bool psig_library_init(void);
[[nodiscard]] bool psig_library_is_running(void);
void psig_library_shutdown(void);

[[nodiscard]] ascii const *psig_library_description(void);

