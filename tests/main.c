#include "libposix_signals/libposix_signals.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
   printf("Running tests for \"%s\"...\n", psignal_library_description());

   assert(psignal_library_init() == true);
   assert(psignal_library_init() == true);
   assert(psignal_library_is_running() == true);
   psignal_library_shutdown();
   assert(psignal_library_is_running() == false);
   psignal_library_shutdown();
   assert(psignal_library_is_running() == false);

   printf("All tests passed !\n");

   return 0;
}
