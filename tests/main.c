#include "libposix_signals/libposix_signals.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
   printf("Running tests for \"%s\"...\n", psig_library_description());

   assert(psig_library_init() == true);
   assert(psig_library_init() == true);
   assert(psig_library_is_running() == true);
   psig_library_shutdown();
   assert(psig_library_is_running() == false);
   psig_library_shutdown();
   assert(psig_library_is_running() == false);

   //
   for (PSignal idx = PSignal_EnumFirst; idx <= PSignal_EnumLast; ++idx)
   {
      int const rawSignal  = psignal_associated_raw_signal(idx);
      char const *name     = psignal_name(idx);
      char const *desc     = psignal_desc(idx);
      char const *type     = psignal_is_standard(idx) ? "STANDARD" : "REAL-TIME";
      char const *hookable = psignal_is_hookable(idx) ? " - HOOKABLE" : "";

      printf("POSIX Signal %2i -> %-15s (%-45s) - %s%s\n", rawSignal, name, desc, type, hookable);
   }
   printf("\nAll tests passed !\n");

   return 0;
}
