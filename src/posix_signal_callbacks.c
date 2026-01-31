#define _GNU_SOURCE

#include "libposix_signals/posix_signal_callbacks.h"
#include "libmacros/macro_utils.h"

#include <assert.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>


// ===============================================================================================
// Internal Data
// ===============================================================================================

enum InitStatus : int
{
   InitStatus_NOT_INIT,
   InitStatus_INITIALIZING,
   InitStatus_INIT,
   InitStatus_SHUTTING_DOWN
};
typedef enum InitStatus InitStatus;

struct CallbackSlot
{
   PSigCallback   callback;
   PSignalBitmask sigBitmask;
};
typedef struct CallbackSlot CallbackSlot;


static CallbackSlot s_slots[PSIG_CALLBACK_MAX_SLOTS_CAPACITY] = {};
static unsigned s_nbSlotsUsed = 0u;

static PSignalBitmask s_callbackedSignals = 0lu;
static atomic_int s_initStatus = InitStatus_NOT_INIT;
static void *s_alternateStack = nullptr;


// ===============================================================================================
// Internal Functions
// ===============================================================================================

[[nodiscard]]
static inline bool has_available_slot(void)
{
   return s_nbSlotsUsed < array_capacity(s_slots);
}

[[nodiscard]] 
static CallbackSlot *slot_try_get(PSigCallback const callback)
{
   for (unsigned i = 0; i < s_nbSlotsUsed; ++i)
   {
      CallbackSlot *slot = &s_slots[i];
      if (slot->callback == callback)
      {
         return slot;
      }
   }
   return nullptr;
}

[[nodiscard]]
static CallbackSlot *slot_register(PSigCallback const callback)
{
   CallbackSlot *slot = &s_slots[s_nbSlotsUsed];
   s_nbSlotsUsed += 1;

   slot->callback = callback;
   slot->sigBitmask = PSIG_BITMASK_NONE;

   return slot;
}

static void slot_remove(PSigCallback const callback)
{
   for (unsigned i = 0; i < s_nbSlotsUsed; ++i)
   {
      if (s_slots[i].callback == callback)
      {
         // We don't care about the order of callbacks,
         // Simply copy the last active slot into the now empty one.
         s_slots[i] = s_slots[s_nbSlotsUsed - 1];
         s_nbSlotsUsed -= 1;
         return;
      }
   }
}

[[nodiscard]]
static CallbackSlot *slot_try_get_or_register(PSigCallback const callback)
{
   CallbackSlot *const slot = slot_try_get(callback);

   if (slot == nullptr && has_available_slot())
   {
      return slot_register(callback);
   }
   return slot;
}

static void sigaction_callback_entry_point(int const sig, siginfo_t *info, void *context)
{
   PSignal psig;
   if (!psignal_from_raw_signal(sig, &psig))
   {
      printf("Unknown signal caught (%i)\n", sig);
      exit(sig);
   }

   PSigHookData const data = (PSigHookData) {
      .psig = psig,
      .sigCode = (info ? info->si_signo : 0)
   };

   for (unsigned i = 0; i < s_nbSlotsUsed; ++i)
   {
      CallbackSlot const *slot = &s_slots[i];
      if (slot->sigBitmask & psig)
      {
         slot->callback(&data);
      }
   }
}


static bool callback_posix_signal(PSignal const psig)
{
   assert((psig & PSIG_BITMASK_UNHOOKABLE_SIGNALS) == 0);

   struct sigaction sa = {};
   sigemptyset(&sa.sa_mask);
   // SA_NODEFER: Allows receiving the same signal during handler.
   // SA_SIGINFO: Uses sa_sigaction handler function instead, more parameters.
   // SA_ONSTACK: Executes signal handler in alternate stack instead of the current one.
   //             Necessary for handling StackOverflow/Segfault/...
   sa.sa_flags = SA_NODEFER | SA_SIGINFO | SA_ONSTACK;
   sa.sa_sigaction = &sigaction_callback_entry_point;

   bool const success = sigaction(psignal_to_raw_signal(psig), &sa, nullptr) == 0;
   if (success)
   {
      s_callbackedSignals |= (1ul << psig);
   }
   return success;
}

static bool uncallback_posix_signal(PSignal const psig)
{
   assert((psig & PSIG_BITMASK_UNHOOKABLE_SIGNALS) == 0);

   struct sigaction sa = {};
   sigemptyset(&sa.sa_mask);
   sa.sa_handler = SIG_DFL;

   bool const success = sigaction(psignal_to_raw_signal(psig), &sa, nullptr) == 0;
   if (success)
   {
      s_callbackedSignals &= ~(1ul << psig);
   }
   return success;
}

static bool refresh_signal_hooks(void)
{
   PSignalBitmask sumCallbackMask = PSIG_BITMASK_NONE;
   for (unsigned idx = 0; idx < s_nbSlotsUsed; ++idx)
   {
      sumCallbackMask |= s_slots[idx].sigBitmask;
   }

   if (sumCallbackMask == s_callbackedSignals)
   {
      // No callback to add/remove.
      return true;
   }

   bool success = true;
   for (PSignal sig = PSignal_First; sig < PSignal_Count; ++sig)
   {
      bool const currentState = (s_callbackedSignals >> sig) & 1;
      bool const desiredState = (sumCallbackMask >> sig) & 1;

      if (!currentState && desiredState)
      {
         // Signal has at least one callback and requires a callback now.
         success &= callback_posix_signal(sig);
      }
      else if (currentState && !desiredState)
      {
         // Signal doesn't have any callback attached to it anymore.
         success &= uncallback_posix_signal(sig);
      }
   }
   return success;
}

static bool reset_signal_hooks(void)
{
   s_nbSlotsUsed = 0;
   return refresh_signal_hooks();
}

[[nodiscard]]
static bool setup_alternate_stack(void)
{
   // TODO: Might be better to use mmap instead of malloc here.
   unsigned const stackSize = SIGSTKSZ;
   s_alternateStack = malloc(stackSize);

   if (s_alternateStack == nullptr)
   {
      return false;
   }

   stack_t const stack = (stack_t) {
      .ss_sp = s_alternateStack,
      .ss_size = stackSize,
      .ss_flags = 0
   };

   return (sigaltstack(&stack, nullptr) == 0);
}

static void stack_restore_default(void)
{
   stack_t ss;
   ss.ss_flags = SS_DISABLE;
   sigaltstack(&ss, nullptr);
   if (s_alternateStack)
   {
      free(s_alternateStack);
      s_alternateStack = nullptr;
   }
}


// ===============================================================================================
// Public API Functions
// ===============================================================================================

bool psignal_callback_system_init(PSigSystemOptions const *options)
{
   InitStatus expected = InitStatus_NOT_INIT;
   if (!atomic_compare_exchange_strong(&s_initStatus, &expected, InitStatus_INITIALIZING))
   {
      return psignal_callback_system_is_init();
   }

   bool success = true;
   if (options && options->useAlternateStack)
   {
      success &= setup_alternate_stack();
   }

   atomic_store(&s_initStatus, success ? InitStatus_INIT : InitStatus_NOT_INIT);
   return psignal_callback_system_is_init();
}


bool psignal_callback_system_is_init(void)
{
   return atomic_load(&s_initStatus) == InitStatus_INIT;
}


void psignal_callback_system_shutdown(void)
{
   InitStatus expected = InitStatus_INIT;
   if (!atomic_compare_exchange_strong(&s_initStatus, &expected, InitStatus_SHUTTING_DOWN))
   {
      return;
   }

   stack_restore_default();
   reset_signal_hooks();

   assert(s_callbackedSignals == PSIG_BITMASK_NONE);
   assert(s_nbSlotsUsed == 0);
   assert(s_alternateStack == nullptr);

   atomic_store(&s_initStatus, InitStatus_NOT_INIT);
}

bool psignal_callback_register(PSigCallback const callback, PSignalBitmask const bitmask)
{
   PSignalBitmask const hookableBitmask = (bitmask & PSIG_BITMASK_HOOKABLE_SIGNALS);
   if (hookableBitmask == PSIG_BITMASK_NONE)
   {
      return false;
   }

   CallbackSlot *const slot = slot_try_get_or_register(callback);
   if (slot != nullptr)
   {
      slot->sigBitmask |= hookableBitmask;
      return refresh_signal_hooks();
   }
   return false;
}

bool psignal_callback_is_registered_on(PSigCallback const callback, PSignalBitmask const bitmask)
{
   PSignalBitmask const hookableBitmask = (bitmask & PSIG_BITMASK_HOOKABLE_SIGNALS);
   if (hookableBitmask == PSIG_BITMASK_NONE)
   {
      return false;
   }

   CallbackSlot *const slot = slot_try_get(callback);
   if (slot)
   {
      return (slot->sigBitmask & hookableBitmask) == hookableBitmask;
   }

   return false;
}

bool psignal_callback_is_registered(PSigCallback const callback)
{
   return slot_try_get(callback) != nullptr;
}

void psignal_callback_unregister(PSigCallback const callback, PSignalBitmask const bitmask)
{
   PSignalBitmask const hookableBitmask = (bitmask & PSIG_BITMASK_HOOKABLE_SIGNALS);
   CallbackSlot *const slot = slot_try_get(callback);
   if (slot)
   {
      PSignalBitmask const currentlyOnToRemove = slot->sigBitmask & hookableBitmask;
      slot->sigBitmask &= ~(currentlyOnToRemove);
      if (slot->sigBitmask == PSIG_BITMASK_NONE)
      {
         slot_remove(callback);
      }
      refresh_signal_hooks();
   }
}
