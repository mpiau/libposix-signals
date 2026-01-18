#pragma once

#include "posix_signals.h"

//================================================================================================
// POSIX Signal Hooks
//================================================================================================

/*
   TODO
*/
[[nodiscard]]
bool psignal_hook_is_authorized(PSignal); // return (psig != PSignal_SIGKILL && psig != PSignal_SIGSTOP);
