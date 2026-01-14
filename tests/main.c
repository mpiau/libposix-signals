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

   printf("All tests passed !\n");

   return 0;
}
