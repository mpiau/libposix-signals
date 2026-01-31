#pragma once

#include "posix_signal_bitmasks.h"


//================================================================================================
// POSIX Signal Callbacks
//================================================================================================

struct PSigSystemOptions
{
   bool useAlternateStack;
};
typedef struct PSigSystemOptions PSigSystemOptions;

struct PSigHookData
{
   PSignal psig;
   int sigCode;
   // TODO to fill with more information given by sigaction's callback.
};
typedef struct PSigHookData PSigHookData;

typedef void (*PSigCallback)(PSigHookData const *);

/*
   Controls the number of callbacks that can be supported at the same time.
   Having the same callback registered on multiple signals (even through multiple calls)
   will still only takes one slot.
*/
static constexpr unsigned PSIG_CALLBACK_MAX_SLOTS_CAPACITY = 16u;


// ===============================================================================================
// Public API Functions
// ===============================================================================================

// TODO: Documentation.

[[nodiscard]] bool psignal_callback_system_init(PSigSystemOptions const *);
[[nodiscard]] bool psignal_callback_system_is_init(void);
void psignal_callback_system_shutdown(void);

// NOTE
// Even if given, the implementation discards automtically SIGKILL/SIGSTOP from the request.
// Do we want that kind of hidden behaviour ?

[[nodiscard]] bool psignal_callback_register(PSigCallback, PSignalBitmask);
[[nodiscard]] bool psignal_callback_is_registered_on(PSigCallback, PSignalBitmask);
[[nodiscard]] bool psignal_callback_is_registered(PSigCallback);
void psignal_callback_unregister(PSigCallback, PSignalBitmask);
