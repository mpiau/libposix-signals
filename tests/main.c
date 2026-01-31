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


int main(void)
{
   printf("Running tests for libposix-signals\n");

   PSigSystemOptions const opts = { .useAlternateStack = true };

   assert(psignal_callback_system_init(&opts));
   assert(psignal_callback_system_init(nullptr));
   assert(psignal_callback_system_is_init());

   for (PSignal idx = PSignal_First; idx < PSignal_Count; ++idx)
   {
      int const rawSignal  = psignal_into_raw_signal(idx);
      char const *name     = psignal_name(idx);
      char const *desc     = psignal_desc(idx);
      char const *type     = psignal_is_standard(idx) ? "STANDARD" : "REAL-TIME";

      printf("POSIX Signal %2i -> %-15s (%-45s) - %s\n", rawSignal, name, desc, type);
   }

   assert(psignal_callback_register(crash_callback, PSIG_BITMASK_FATAL_SIGNALS));

   PSignalBitmask const SigintMask = (1ul << PSignal_SIGINT);
   assert(psignal_callback_update(crash_callback, PSIG_BITMASK_FATAL_SIGNALS ^ SigintMask));
   assert(!psignal_callback_is_registered_on(crash_callback, SigintMask));

   assert(psignal_callback_update(crash_callback, PSIG_BITMASK_FATAL_SIGNALS));
   assert(psignal_callback_is_registered_on(crash_callback, PSIG_BITMASK_FATAL_SIGNALS));

   printf("Raising hooked SIGINT...\n");
   assert(psignal_raise(PSignal_SIGINT));
   assert(sigintReceived == 1);
   printf("Raising hooked SIGSEGV...\n");
   assert(psignal_raise(PSignal_SIGSEGV));
   assert(sigsegvReceived == 1);
   printf("Raising non-hooked SIGCONT...\n");
   assert(psignal_raise(PSignal_SIGCONT));
   assert(sigcontReceived == 0);

   assert(psignal_callback_is_registered(crash_callback));
   psignal_callback_unregister(crash_callback);
   assert(!psignal_callback_is_registered_on(crash_callback, PSIG_BITMASK_FATAL_SIGNALS));
   assert(!psignal_callback_is_registered(crash_callback));

   psignal_callback_system_shutdown();
   assert(psignal_callback_system_is_init() == false);
   psignal_callback_system_shutdown();
   assert(psignal_callback_system_is_init() == false);

   printf("\nAll tests passed !\n");

   return 0;
}
