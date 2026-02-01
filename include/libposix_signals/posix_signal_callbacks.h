#pragma once

#include "posix_signal_emission_reasons.h"
#include "posix_signal_masks.h"


//================================================================================================
// POSIX Signal Callbacks
//================================================================================================

struct PSigHookData
{
   PSignal     psig;
   PSignalCode code;
   // TODO to fill with more information given by sigaction's callback.
};
typedef struct PSigHookData PSigHookData;

typedef void (*PSignalCallback)(PSigHookData const *);

/*
   Controls the number of callbacks that can be supported at the same time.
   Having the same callback registered on multiple signals (even through multiple calls)
   will still only takes one slot.
*/
static constexpr unsigned PSIG_CALLBACK_MAX_SLOTS_CAPACITY = 16u;


// ===============================================================================================
// Public API Functions
// ===============================================================================================

/*
   IMPORTANT: These functions are not thread-safe.
   Even if explicitly specified, it's not possible to hook SIGKILL and SIGSTOP.
   However, the API is aware of that and will simply ignore them. Treating these as errors when
   specified in a mask would be too cumbersome for the user.
*/

//------------------------------------------------------------------------------------------------
// Signal Stack handling functions
//------------------------------------------------------------------------------------------------

/*
   Uses an alternate signal stack (allocated on the heap) to handle signals.
   Necessary when handling stack-overflow or crash related signals.
*/
bool psignal_setup_alternate_stack(void);

/*
   Restores the default stack behaviour.
*/
bool psignal_restore_default_stack(void);


//------------------------------------------------------------------------------------------------
// Callback Registration functions
//------------------------------------------------------------------------------------------------

/*
   Returns true if there is at least one available slot to register a new callback.
*/
[[nodiscard]] bool psignal_callback_has_available_slot(void);

/*
   Registers a callback to the system.
   It's not possible to register the same callback multiple times but will still
   returns true as the callback will be registered in the system.
*/
bool psignal_callback_register(PSignalCallback);

/*
   Checks if a particular callback is registered in the system.
*/
[[nodiscard]] bool psignal_callback_is_registered(PSignalCallback);

/*
   Unregisters the given callback from the system.
*/
void psignal_callback_unregister(PSignalCallback);

/*
   Unregisters all callbacks.
*/
void psignal_callback_unregister_all(void);


//------------------------------------------------------------------------------------------------
// Callback Hook addition/substraction functions.
//------------------------------------------------------------------------------------------------

/*
   Hooks the callback to a particular signal.
   The other existing hooks associated with the callback are left unchanged.
*/
bool psignal_callback_attach_signal(PSignalCallback, PSignal);

/*
   Hooks the callback the set of signals specified in the mask.
   The other existing hooks associated with the callback are left unchanged.
*/
bool psignal_callback_attach_mask(PSignalCallback, PSignalMask);

/*
   Unhooks the callback from a particular signal.
   The other existing hooks associated with the callback are left unchanged.
*/
void psignal_callback_detach_signal(PSignalCallback, PSignal);

/*
   Unhooks the callback from a particular set of signals specified in the mask.
   The other existing hooks associated with the callback are left unchanged.
*/
void psignal_callback_detach_mask(PSignalCallback, PSignalMask);

/*
   Returns true if the callback currently has a hook on the given signal.
*/
[[nodiscard]] bool psignal_callback_is_signal_attached(PSignalCallback, PSignal);

/*
   Returns true only if the callback has a hook on the whole set of signals specified
   in the mask.
*/
[[nodiscard]] bool psignal_callback_is_mask_attached(PSignalCallback, PSignalMask);
