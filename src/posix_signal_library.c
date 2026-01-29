#include "libposix_signals/posix_signal_library.h"
#include "libposix_signals/posix_signals.h"

#include "../src/internal.h"

#include <stdatomic.h>


enum LibStatus : char
{
   LibStatus_NOT_INITIALIZED,
   LibStatus_INITIALIZING,
   LibStatus_RUNNING,
   LibStatus_SHUTTING_DOWN
};
typedef enum LibStatus LibStatus;


static atomic_char s_libStatus = LibStatus_NOT_INITIALIZED;


bool psignal_library_init(void)
{
   char expected = LibStatus_NOT_INITIALIZED;
   char const desired = LibStatus_INITIALIZING;

   if (atomic_compare_exchange_strong(&s_libStatus, &expected, desired))
   {
      bool const success = psignal_callback_internal_init();
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
   char expected = LibStatus_RUNNING;
   char const desired = LibStatus_SHUTTING_DOWN;

   if (atomic_compare_exchange_strong(&s_libStatus, &expected, desired))
   {
      psignal_callback_internal_shutdown();
      atomic_store(&s_libStatus, LibStatus_NOT_INITIALIZED);
   }
}

char const *psignal_library_description(void)
{
   return "POSIX Signals Library";
}
