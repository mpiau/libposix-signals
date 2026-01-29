#define _GNU_SOURCE

#include "libposix_signals/posix_signal_callbacks.h"
#include "libposix_signals/posix_signal_dispositions.h"
#include "libposix_signals/posix_signals.h"

#include "libmacros/macro_utils.h"

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


// ===============================================================================================
// Internal Data
// ===============================================================================================

typedef struct CallbackSlot
{
   PSigCallback callback;
   PSignalMask  hookedMask;
} CallbackSlot;

static CallbackSlot s_cbSlots[PSIG_CALLBACKS_MAX_CAPACITY] = {};
static unsigned s_cbSlotsUsed = 0u;


// ===============================================================================================
// Internal Functions
// ===============================================================================================

[[nodiscard]]
static inline bool has_available_slot(void)
{
   return s_cbSlotsUsed < array_capacity(s_cbSlots);
}

[[nodiscard]]
static inline bool is_signal_hooked(PSignalMask const mask, PSignal const psig)
{
   return (mask >> psig) & 1;
}

[[nodiscard]] 
static CallbackSlot *try_get_slot(PSigCallback const cb)
{
   for (unsigned i = 0; i < s_cbSlotsUsed; ++i)
   {
      CallbackSlot *regCb = &s_cbSlots[i];
      if (regCb->callback == cb)
      {
         return regCb;
      }
   }

   return nullptr;
}

[[nodiscard]]
static CallbackSlot *register_new_slot(PSigCallback const cb)
{
   assert(!try_get_slot(cb));
   assert(has_available_slot());

   CallbackSlot *regCb = &s_cbSlots[s_cbSlotsUsed];

   regCb->callback = cb;
   regCb->hookedMask = psignal_disposition_mask_none();

   s_cbSlotsUsed += 1;

   return regCb;
}

static void remove_from_slot(PSigCallback const cb)
{
   assert(s_cbSlotsUsed > 0);

   for (unsigned i = 0; i < s_cbSlotsUsed; ++i)
   {
      if (s_cbSlots[i].callback == cb)
      {
         // As we don't care about the order of callbacks,
         // simply copy the last slot into the removed one.
         s_cbSlots[i] = s_cbSlots[s_cbSlotsUsed - 1];
         s_cbSlotsUsed -= 1;
         return;
      }
   }

   // Note:
   // Slot not found, do we want to emit an error here or simply discard that fact ?
}

[[nodiscard]]
static CallbackSlot *try_get_or_register_new_slot(PSigCallback const cb)
{
   CallbackSlot *regCb = try_get_slot(cb);

   if (regCb == nullptr && has_available_slot())
   {
      regCb = register_new_slot(cb);
   }

   return regCb;
}

[[nodiscard]]
static bool upgrade_slot(PSigCallback const cb, PSignalMask const mask)
{
   CallbackSlot *regCb = try_get_or_register_new_slot(cb);
   if (regCb != nullptr)
   {
      regCb->hookedMask |= mask;
      return true;
   }

   return false;
}

static void downgrade_slot(PSigCallback const cb, PSignalMask const mask)
{
   CallbackSlot *regCb = try_get_slot(cb);
   if (regCb)
   {
      regCb->hookedMask &= ~(mask);
      if (regCb->hookedMask == psignal_disposition_mask_none())
      {
         remove_from_slot(cb);
      }
   }
}

[[nodiscard]]
static bool setup_alternate_stack(void)
{
   unsigned const stackSize = SIGSTKSZ;
   void *const stackBuffer = malloc(stackSize);

   if (stackBuffer == nullptr)
   {
      return false;
   }

   stack_t const stack = (stack_t) {
      .ss_sp = stackBuffer,
      .ss_size = stackSize,
      .ss_flags = 0
   };

   return (sigaltstack(&stack, nullptr) == 0);
}

static void sigaction_callback_entry_point(int const sig, siginfo_t *info, void *context)
{
   PSignal psig;
   if (!psignal_from_raw_signal(sig, &psig))
   {
      printf("Unknown signal caught (%i)\n", sig);
      exit(sig);
   }

   PSigCallbackInfo const cbInfo = (PSigCallbackInfo) {
      .sig = psig,
      .sigCode = (info ? info->si_signo : 0)
   };

   for (unsigned i = 0; i < s_cbSlotsUsed; ++i)
   {
      CallbackSlot const *regCb = &s_cbSlots[i];
      if (is_signal_hooked(regCb->hookedMask, psig))
      {
         regCb->callback(&cbInfo);
      }
   }
}

// ===============================================================================================
// Internal API Functions
// ===============================================================================================

bool psignal_callback_internal_init(void)
{
   if (!setup_alternate_stack())
      return false;

   struct sigaction sa = {};
   sigemptyset(&sa.sa_mask);
   // SA_NODEFER: Allows receiving the same signal during handler.
   // SA_SIGINFO: Uses sa_sigaction handler function instead, more parameters.
   // SA_ONSTACK: Executes signal handler in alternate stack instead of the current one.
   //             Necessary for handling StackOverflow/Segfault/...
   sa.sa_flags = SA_NODEFER | SA_SIGINFO | SA_ONSTACK;
   sa.sa_sigaction = &sigaction_callback_entry_point;

   for (PSignal idx = PSignal_ENUM_FIRST; idx <= PSignal_ENUM_LAST; ++idx)
   {
      if (!psignal_callback_is_authorized(idx))
         continue;

      int const rawSignal = psignal_to_raw_signal(idx);
      if (sigaction(rawSignal, &sa, nullptr) != 0)
      {
         fprintf(stderr, "ERROR - Failure to hook callback on \"%s\" (%i).\n", psignal_name(idx), rawSignal);
         return false;
      }
   }
   return true;
}

void psignal_callback_internal_shutdown(void)
{
   struct sigaction sa = {};
   sigemptyset(&sa.sa_mask);
   sa.sa_handler = SIG_DFL;

   for (PSignal idx = PSignal_ENUM_FIRST; idx <= PSignal_ENUM_LAST; ++idx)
   {
      if (psignal_callback_is_authorized(idx))
      {
         sigaction(psignal_to_raw_signal(idx), &sa, nullptr);
      }
   }

   s_cbSlotsUsed = 0;
}


// ===============================================================================================
// Public API Functions
// ===============================================================================================

bool psignal_callback_is_authorized(PSignal const psig)
{
   return (psig != PSignal_SIGKILL && psig != PSignal_SIGSTOP);
}

bool psignal_callback_is_hooked_on(PSignal const psig, PSigCallback const cb)
{
   CallbackSlot const *regCb = try_get_slot(cb);
   return (regCb != nullptr) ? is_signal_hooked(regCb->hookedMask, psig) : false;
}


bool psignal_callback_hook_on_sig(PSignal const psig, PSigCallback const cb)
{
   return upgrade_slot(cb, (1lu << psig));
}

bool psignal_callback_hook_on_disposition(PSigDisposition const disp, PSigCallback const cb)
{
   return upgrade_slot(cb, psignal_disposition_mask(disp));
}

bool psignal_callback_hook_on_all(PSigCallback const cb)
{
   return upgrade_slot(cb, psignal_disposition_mask_all());
}


void psignal_callback_remove_from_sig(PSignal const psig, PSigCallback const cb)
{
   downgrade_slot(cb, (1lu << psig));
}

void psignal_callback_remove_from_disposition(PSigDisposition const disp, PSigCallback const cb)
{
   downgrade_slot(cb, (psignal_disposition_mask(disp)));
}

void psignal_callback_remove_from_all(PSigCallback const cb)
{
   downgrade_slot(cb, psignal_disposition_mask_all());
}
