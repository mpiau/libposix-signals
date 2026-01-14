#pragma once

#include "posix_signal_enum.h"
#include "posix_signal_types.h"

// PSignal -> si_signo from siginfo_t
// sigCode -> si_code  from siginfo_t (the reason why the signal was sent)
[[nodiscard]] __attribute__((__returns_nonnull__))
ascii const *psig_emission_reason(PSignal, sigCode);
