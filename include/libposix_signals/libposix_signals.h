#pragma once

#include "posix_signal_dispositions.h"
#include "posix_signal_emission_reason.h"
#include "posix_signals.h"


//================================================================================================
// POSIX Signals Library
//================================================================================================

[[nodiscard]] bool psig_library_init(void);
[[nodiscard]] bool psig_library_is_running(void);
void psig_library_shutdown(void);

[[nodiscard]] char const *psig_library_description(void);

