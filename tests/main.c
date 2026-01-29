#include "libposix_signals/libposix_signals.h"

#include <assert.h>
#include <stdio.h>
#include <signal.h>

static sig_atomic_t sigintReceived = 0;
static sig_atomic_t sigsegvReceived = 0;
static sig_atomic_t sigcontReceived = 0;

void crash_callback(PSigCallbackInfo const *info)
{
   printf("Crash callback called with signal %s (%i)\n",
      psignal_name(info->sig), psignal_to_raw_signal(info->sig)
   );

   switch (info->sig)
   {
      case PSignal_SIGINT: sigintReceived += 1; break;
      case PSignal_SIGSEGV: sigsegvReceived += 1; break;
      case PSignal_SIGCONT: sigcontReceived += 1; break;
      default: break;
   }
}


int main(void)
{
   printf("Running tests for \"%s\"...\n", psignal_library_description());

   assert(psignal_library_init() == true);
   assert(psignal_library_init() == true);
   assert(psignal_library_is_running() == true);

   for (PSignal idx = PSignal_ENUM_FIRST; idx <= PSignal_ENUM_LAST; ++idx)
   {
      int const rawSignal  = psignal_to_raw_signal(idx);
      char const *name     = psignal_name(idx);
      char const *desc     = psignal_desc(idx);
      char const *type     = psignal_is_standard(idx) ? "STANDARD" : "REAL-TIME";

      printf("POSIX Signal %2i -> %-15s (%-45s) - %s\n", rawSignal, name, desc, type);
   }

   assert(psignal_callback_hook_on_disposition(PSigDisposition_TERMINATE, crash_callback));
   assert(psignal_callback_hook_on_disposition(PSigDisposition_CORE_DUMP, crash_callback));
   assert(psignal_callback_hook_on_disposition(PSigDisposition_STOP, crash_callback));
   for (PSignal idx = PSignal_ENUM_FIRST; idx <= PSignal_ENUM_LAST; ++idx)
   {
      PSigDisposition const disp = psignal_disposition_default(idx);
      bool const shouldBeHooked =
          ( disp == PSigDisposition_TERMINATE
         || disp == PSigDisposition_CORE_DUMP
         || disp == PSigDisposition_STOP);
      assert(psignal_callback_is_hooked_on(idx, crash_callback) == shouldBeHooked);
   }

   psignal_callback_remove_from_sig(PSignal_SIGINT, crash_callback);
   assert(!psignal_callback_is_hooked_on(PSignal_SIGINT, crash_callback));

   assert(psignal_callback_hook_on_sig(PSignal_SIGINT, crash_callback));
   assert(psignal_callback_is_hooked_on(PSignal_SIGINT, crash_callback));

   printf("Raising hooked SIGINT...\n");
   assert(psignal_raise(PSignal_SIGINT));
   assert(sigintReceived == 1);
   printf("Raising hooked SIGSEGV...\n");
   assert(psignal_raise(PSignal_SIGSEGV));
   assert(sigsegvReceived == 1);
   printf("Raising non-hooked SIGCONT...\n");
   assert(psignal_raise(PSignal_SIGCONT));
   assert(sigcontReceived == 0);

   psignal_callback_remove_from_all(crash_callback);
   for (PSignal idx = PSignal_ENUM_FIRST; idx <= PSignal_ENUM_LAST; ++idx)
   {
      assert(!psignal_callback_is_hooked_on(idx, crash_callback));
   }


   psignal_library_shutdown();
   assert(psignal_library_is_running() == false);
   psignal_library_shutdown();
   assert(psignal_library_is_running() == false);

   printf("\nAll tests passed !\n");

   return 0;
}
