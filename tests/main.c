#include "libposix_signals/libposix_signals.h"

#include <assert.h>
#include <stdio.h>
#include <signal.h>

static sig_atomic_t sigintReceived = 0;
static sig_atomic_t sigsegvReceived = 0;
static sig_atomic_t sigcontReceived = 0;

void crash_callback(PSigHookData const *data)
{
   printf("Crash callback called with signal %s (%i)\n",
      psignal_name(data->psig), psignal_into_raw_signal(data->psig)
   );

   switch (data->psig)
   {
      case PSignal_SIGINT: sigintReceived += 1; break;
      case PSignal_SIGSEGV: sigsegvReceived += 1; break;
      case PSignal_SIGCONT: sigcontReceived += 1; break;
      default: break;
   }
}

void crash_callback_two(PSigHookData const *data)
{
   printf("Crash callback 2 called with signal %s (%i)\n",
      psignal_name(data->psig), psignal_into_raw_signal(data->psig)
   );

   switch (data->psig)
   {
      case PSignal_SIGINT: sigintReceived += 1; break;
      case PSignal_SIGSEGV: sigsegvReceived += 1; break;
      case PSignal_SIGCONT: sigcontReceived += 1; break;
      default: break;
   }
}


int main(void)
{
   printf("Running tests for libposix-signals\n");

   assert(psignal_setup_alternate_stack());
   assert(psignal_setup_alternate_stack()); // Should be ignored

   for (PSignal idx = PSignal_First; idx < PSignal_Count; ++idx)
   {
      int const rawSignal  = psignal_into_raw_signal(idx);
      char const *name     = psignal_name(idx);
      char const *desc     = psignal_desc(idx);
      char const *type     = psignal_is_standard(idx) ? "STANDARD" : "REAL-TIME";

      printf("POSIX Signal %2i -> %-15s (%-45s) - %s\n", rawSignal, name, desc, type);
   }

   assert(!psignal_callback_register(nullptr));
   assert(!psignal_callback_attach_mask(nullptr, PSignalMask_FATAL_SIGNALS));


   assert(psignal_callback_register(crash_callback));
   assert(psignal_callback_attach_mask(crash_callback, PSignalMask_FATAL_SIGNALS));

   psignal_callback_detach_signal(crash_callback, PSignal_SIGINT);
   assert(!psignal_callback_is_signal_attached(crash_callback, PSignal_SIGINT));
   assert(psignal_callback_is_signal_attached(crash_callback, PSignal_SIGHUP));
   assert(!psignal_callback_is_mask_attached(crash_callback, PSignalMask_FATAL_SIGNALS));

   assert(psignal_callback_attach_signal(crash_callback, PSignal_SIGINT));
   assert(psignal_callback_is_mask_attached(crash_callback, PSignalMask_FATAL_SIGNALS));

   printf("Raising hooked SIGINT...\n");
   assert(psignal_raise(PSignal_SIGINT));
   assert(sigintReceived == 1);
   printf("Raising hooked SIGSEGV...\n");
   assert(psignal_raise(PSignal_SIGSEGV));
   assert(sigsegvReceived == 1);
   printf("Raising non-hooked SIGCONT...\n");
   assert(psignal_raise(PSignal_SIGCONT));
   assert(sigcontReceived == 0);

   printf("Registering another callback without SIGSEGV...\n");
   assert(psignal_callback_register(crash_callback_two));
   assert(psignal_callback_attach_mask(crash_callback_two, PSignalMask_FATAL_SIGNALS));
   psignal_callback_detach_signal(crash_callback_two, PSignal_SIGSEGV);

   printf("Raising double-hooked SIGINT...\n");
   assert(psignal_raise(PSignal_SIGINT));
   assert(sigintReceived == 3);
   printf("Raising hooked SIGSEGV...\n");
   assert(psignal_raise(PSignal_SIGSEGV));
   assert(sigsegvReceived == 2);
   printf("Raising non-hooked SIGCONT...\n");
   assert(psignal_raise(PSignal_SIGCONT));
   assert(sigcontReceived == 0);

   // Unregister - Register again, Mask should be reset
   assert(psignal_callback_is_registered(crash_callback));
   psignal_callback_unregister(crash_callback);
   assert(!psignal_callback_is_registered(crash_callback));
   psignal_callback_register(crash_callback);
   assert(!psignal_callback_is_mask_attached(crash_callback, PSignalMask_FATAL_SIGNALS));
   assert(psignal_callback_is_mask_attached(crash_callback, PSignalMask_NONE));

   psignal_callback_unregister_all();
   assert(!psignal_callback_is_registered(crash_callback));
   assert(!psignal_callback_is_registered(crash_callback_two));

   assert(psignal_restore_default_stack());
   assert(psignal_restore_default_stack()); // Should be ignored.

   printf("\nAll tests passed !\n");

   return 0;
}
