#pragma once

#include "posix_signals.h"

// sigCode -> si_code  from siginfo_t (the reason why the signal was sent)
// Always return non-null string
[[nodiscard]] char const *psig_emission_reason(PSignal, int sigCode);
