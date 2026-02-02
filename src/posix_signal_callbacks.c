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

struct PSignalSlot
{
   PSignalCallback callback;
   PSignalMask     mask;
};
typedef struct PSignalSlot PSignalSlot;


static PSignalSlot s_slots[PSIG_CALLBACK_MAX_SLOTS_CAPACITY] = {};
static unsigned s_nbSlotsUsed = 0u; 

static PSignalMask s_hookedSignals = 0ul;

static stack_t s_alternateStack = {};
static stack_t s_defaultStack   = {};


// ===============================================================================================
// Internal Functions
// ===============================================================================================

[[nodiscard]]
static PSignalMask sum_slots_mask(void)
{
   PSignalMask mask = PSignalMask_NONE;
   for (unsigned idx = 0; idx < s_nbSlotsUsed; ++idx)
   {
      mask |= s_slots[idx].mask;
   }

   return mask;
}

[[nodiscard]]
static PSignalSlot *slot_try_get(PSignalCallback const cb)
{
   if (cb == nullptr)
   {
      return nullptr;
   }

   for (unsigned idx = 0; idx < s_nbSlotsUsed; ++idx)
   {
      if (s_slots[idx].callback == cb)
      {
         return &s_slots[idx];
      }
   }
   return nullptr;
}

static PSignalSlot *slot_try_get_or_register(PSignalCallback const cb)
{
   PSignalSlot *slot = slot_try_get(cb);
   if (slot == nullptr && cb != nullptr && psignal_callback_has_available_slot())
   {
      slot = &s_slots[s_nbSlotsUsed];
      slot->callback = cb;
      slot->mask = PSignalMask_NONE;
      s_nbSlotsUsed += 1;
   }
   return slot;
}

static void sigaction_callback_entry_point(int const sig, siginfo_t *const info, void *const context)
{
   PSignal psig;
   if (!psignal_from_raw_signal(sig, &psig))
   {
      printf("Unknown signal caught (%i)\n", sig);
      exit(sig);
   }

   PSignalHookData const data = (PSignalHookData) {
      .psig = psig,
      .code = (info ? info->si_signo : 0),
      .info = (void const *)info,
      .context = (void const *)context
   };

   for (unsigned idx = 0; idx < s_nbSlotsUsed; ++idx)
   {
      if (s_slots[idx].mask & (1ul << psig))
      {
         s_slots[idx].callback(&data);
      }
   }
}

static bool hook_posix_signal(PSignal const psig)
{
   struct sigaction sa = {};
   sigemptyset(&sa.sa_mask);
   // SA_NODEFER: Allows receiving the same signal during handler.
   // SA_SIGINFO: Uses sa_sigaction handler function instead, more parameters.
   // SA_ONSTACK: Executes signal handler in alternate stack instead of the current one.
   //             Necessary for handling StackOverflow/Segfault/...
   sa.sa_flags = SA_NODEFER | SA_SIGINFO | SA_ONSTACK;
   sa.sa_sigaction = &sigaction_callback_entry_point;

   bool const success = sigaction(psignal_into_raw_signal(psig), &sa, nullptr) == 0;
   if (success)
   {
      s_hookedSignals |= (1ul << psig);
   }
   return success;
}

static bool unhook_posix_signal(PSignal const psig)
{
   struct sigaction sa = {};
   sigemptyset(&sa.sa_mask);
   sa.sa_handler = SIG_DFL;

   bool const success = sigaction(psignal_into_raw_signal(psig), &sa, nullptr) == 0;
   if (success)
   {
      s_hookedSignals &= ~(1ul << psig);
   }
   return success;
}

static void refresh_signal_hooks(void)
{
   // Remove all unhookable signals from the mask.
   // It would just waste CPU cycle to try for nothing to hook them.
   PSignalMask const slotsHookableMask = (sum_slots_mask() & PSignalMask_HOOKABLE_SIGNALS);
   if (slotsHookableMask == s_hookedSignals)
   {
      // No hook to add/remove.
      return;
   }

   for (PSignal psig = PSignal_First; psig < PSignal_Count; ++psig)
   {
      if (psig == PSignal_SIGKILL || psig == PSignal_SIGSTOP)
      {
         continue;
      }
      bool const currentState = (s_hookedSignals   >> psig) & 1;
      bool const desiredState = (slotsHookableMask >> psig) & 1;

      if (!currentState && desiredState)
      { // At least one callback attached to the signal, hook required.
         hook_posix_signal(psig);
      }
      else if (currentState && !desiredState)
      { // No callback attached to the signal, hook not needed anymore.
         unhook_posix_signal(psig);
      }
   }
}


// ===============================================================================================
// Public API Functions
// ===============================================================================================

//------------------------------------------------------------------------------------------------
// Signal Stack handling functions
//------------------------------------------------------------------------------------------------

bool psignal_setup_alternate_stack(void)
{
   if (s_alternateStack.ss_sp != nullptr)
   { // Already an alternate stack in place.
      return true;
   }

   unsigned const stackSize = SIGSTKSZ;
   s_alternateStack.ss_sp = malloc(stackSize);

   if (s_alternateStack.ss_sp == nullptr)
   {
      return false;
   }
   s_alternateStack.ss_size = stackSize;

   if (sigaltstack(&s_alternateStack, &s_defaultStack) != 0)
   {
      free(s_alternateStack.ss_sp);
      s_alternateStack.ss_size = 0;
      return false;
   }

   return true;
}

bool psignal_restore_default_stack(void)
{
   if (s_alternateStack.ss_sp == nullptr)
   { // No alternate stack currently in place.
      return true;
   }
   else if (sigaltstack(&s_defaultStack, nullptr) != 0)
   {
      return false;
   }

   free(s_alternateStack.ss_sp);
   s_alternateStack.ss_sp = nullptr;
   s_alternateStack.ss_size = 0;
   return true;
}


//------------------------------------------------------------------------------------------------
// Callback Registration functions
//------------------------------------------------------------------------------------------------

bool psignal_callback_has_available_slot(void)
{
   return s_nbSlotsUsed < array_capacity(s_slots);
}

bool psignal_callback_is_registered(PSignalCallback const cb)
{
   return slot_try_get(cb) != nullptr;
}

void psignal_callback_unregister(PSignalCallback const cb)
{
   PSignalSlot *const slot = slot_try_get(cb);
   if (slot)
   {
      *slot = s_slots[s_nbSlotsUsed - 1];
      s_nbSlotsUsed -= 1;
      refresh_signal_hooks();
   }
}

void psignal_callback_unregister_all(void)
{
   s_nbSlotsUsed = 0;
   refresh_signal_hooks();
}


//------------------------------------------------------------------------------------------------
// Callback Hook addition/substraction functions.
//------------------------------------------------------------------------------------------------

bool psignal_callback_attach_signal(PSignalCallback const cb, PSignal const psig)
{
   return psignal_callback_attach_mask(cb, (1ul << psig));
}

bool psignal_callback_attach_mask(PSignalCallback const cb, PSignalMask const mask)
{
   PSignalSlot *const slot = slot_try_get_or_register(cb);
   if (slot != nullptr)
   {
      slot->mask |= mask;
      refresh_signal_hooks();
      return true;
   }
   return false;
}

void psignal_callback_detach_signal(PSignalCallback const cb, PSignal const psig)
{
   psignal_callback_detach_mask(cb, (1ul << psig));
}

void psignal_callback_detach_mask(PSignalCallback const cb, PSignalMask const mask)
{
   PSignalSlot *const slot = slot_try_get(cb);
   if (slot != nullptr)
   {
      slot->mask &= ~(mask);
      refresh_signal_hooks();
   }
}

bool psignal_callback_is_signal_attached(PSignalCallback const cb, PSignal const psig)
{
   return psignal_callback_is_mask_attached(cb, (1ul << psig));
}

bool psignal_callback_is_mask_attached(PSignalCallback const cb, PSignalMask const mask)
{
   PSignalSlot *const slot = slot_try_get(cb);
   return slot && ((slot->mask & mask) == mask);
}
