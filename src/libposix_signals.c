#include "libposix_signals/libposix_signals.h"

#include <stdatomic.h>


enum LibStatus
{
   LibStatus_NOT_INITIALIZED,
   LibStatus_INITIALIZING,
   LibStatus_RUNNING,
   LibStatus_SHUTTING_DOWN
};
typedef enum LibStatus LibStatus;


static atomic_int s_libStatus = LibStatus_NOT_INITIALIZED;


bool psignal_library_init(void)
{
   int expected = LibStatus_NOT_INITIALIZED;
   int const desired = LibStatus_INITIALIZING;

   if (atomic_compare_exchange_strong(&s_libStatus, &expected, desired))
   {
      // TODO the initialization
      bool const success = true;

      atomic_store(&s_libStatus, success ? LibStatus_RUNNING : LibStatus_NOT_INITIALIZED);
      return success;
   }
   else
   {
      return psignal_library_is_running();
   }
}

bool psignal_library_is_running(void)
{
   return atomic_load(&s_libStatus) == LibStatus_RUNNING;
}

void psignal_library_shutdown(void)
{
   int expected = LibStatus_RUNNING;
   int const desired = LibStatus_SHUTTING_DOWN;

   if (atomic_compare_exchange_strong(&s_libStatus, &expected, desired))
   {
      // Shutdown things that has been used.
      atomic_store(&s_libStatus, LibStatus_NOT_INITIALIZED);
   }
}

char const *psignal_library_description(void)
{
   return "POSIX Signals Library";
}
