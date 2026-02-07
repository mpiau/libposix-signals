#pragma once

#include "posix_signal_emission_reasons.h"
#include "posix_signal_masks.h"


//================================================================================================
// POSIX Signal Callbacks
//================================================================================================

/*
   Note: Simpler & safer for the moment to give the received structs from the hook instead of
   extracting their content.
*/

struct PSignalHookData
{
   PSignal     psig;
   PSignalCode code;
   void const *info; // siginfo_t
   void const *context; // ucontext_t
};
typedef struct PSignalHookData PSignalHookData;

typedef void (*PSignalCallback)(PSignalHookData const *);

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
   IMPORTANT:
   - These functions are not thread-safe.
   - A callback can stay registered even without being attached to any signal.
   - You can't register a nullptr callback.
   - Even if explicitly specified, it's not possible to hook SIGKILL and SIGSTOP.
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
   Checks if a particular callback is registered in the system.
*/
[[nodiscard]] bool psignal_callback_is_registered(PSignalCallback);

/*
   Registers given callback.
*/
bool psignal_callback_register(PSignalCallback);

/*
   Unregisters given callback.
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
   Register the callback if not registered yet in the system.
   The other existing hooks associated with the callback are left unchanged.
*/
bool psignal_callback_attach(PSignalCallback, PSignal);

/*
   Hooks the callback the set of signals specified in the mask.
   Register the callback if not registered yet in the system.
   The other existing hooks associated with the callback are left unchanged.
*/
bool psignal_callback_attach_mask(PSignalCallback, PSignalMask);

/*
   Unhooks the callback from a particular signal.
   The other existing hooks associated with the callback are left unchanged.
*/
void psignal_callback_detach(PSignalCallback, PSignal);

/*
   Unhooks the callback from a particular set of signals specified in the mask.
   The other existing hooks associated with the callback are left unchanged.
*/
void psignal_callback_detach_mask(PSignalCallback, PSignalMask);

/*
   Returns true if the callback currently has a hook on the given signal.
*/
[[nodiscard]] bool psignal_callback_is_attached(PSignalCallback, PSignal);

/*
   Returns true only if the callback has a hook on the whole set of signals specified
   in the mask.
*/
[[nodiscard]] bool psignal_callback_is_attached_mask(PSignalCallback, PSignalMask);
