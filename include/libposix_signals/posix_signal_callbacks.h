#pragma once

#include "posix_signal_dispositions.h"
#include "posix_signals.h"


//================================================================================================
// POSIX Signal Callbacks
//================================================================================================

typedef struct PSigCallbackInfo
{
   PSignal sig;
   int sigCode;
   // TODO to fill with more information given by sigaction's callback.
} PSigCallbackInfo;

typedef void (*PSigCallback)(PSigCallbackInfo const *);

/*
   Controls the number of callbacks that can be supported at the same time.
   Having the same callback hooked on multiple signals (even through multiple calls) only count
   as one.
*/
static constexpr unsigned PSIG_CALLBACKS_MAX_CAPACITY = 16u;

// ===============================================================================================
// Public API Functions
// ===============================================================================================

[[nodiscard]] bool psignal_callback_is_authorized(PSignal);
[[nodiscard]] bool psignal_callback_is_hooked_on(PSignal, PSigCallback);

[[nodiscard]] bool psignal_callback_hook_on_sig(PSignal, PSigCallback);
[[nodiscard]] bool psignal_callback_hook_on_disposition(PSigDisposition, PSigCallback);
[[nodiscard]] bool psignal_callback_hook_on_all(PSigCallback);

void psignal_callback_remove_from_sig(PSignal, PSigCallback);
void psignal_callback_remove_from_disposition(PSigDisposition, PSigCallback);
void psignal_callback_remove_from_all(PSigCallback);
