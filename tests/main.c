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
   for (PSignal idx = PSignal_ENUM_FIRST; idx <= PSignal_ENUM_LAST; ++idx)
   {
      int const rawSignal  = psignal_to_raw_signal(idx);
      char const *name     = psignal_name(idx);
      char const *desc     = psignal_desc(idx);
      char const *type     = psignal_is_standard(idx) ? "STANDARD" : "REAL-TIME";

      printf("POSIX Signal %2i -> %-15s (%-45s) - %s\n", rawSignal, name, desc, type);
   }
   printf("\nAll tests passed !\n");

   return 0;
}
