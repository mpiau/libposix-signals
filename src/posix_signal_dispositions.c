#include "libposix_signals/posix_signal_dispositions.h"
#include "libposix_signals/posix_signals.h"
#include "libmacros/macro_utils.h"

#include <stddef.h>


//================================================================================================
// Internal Data
//================================================================================================

static constexpr PSigDisposition S_PSIG_STD_DISPS[] =
{
     [PSignal_SIGHUP]    = PSigDisposition_TERMINATE
   , [PSignal_SIGINT]    = PSigDisposition_TERMINATE
   , [PSignal_SIGQUIT]   = PSigDisposition_CORE_DUMP
   , [PSignal_SIGILL]    = PSigDisposition_CORE_DUMP
   , [PSignal_SIGTRAP]   = PSigDisposition_CORE_DUMP
   , [PSignal_SIGABRT]   = PSigDisposition_CORE_DUMP
   , [PSignal_SIGBUS]    = PSigDisposition_CORE_DUMP
   , [PSignal_SIGFPE]    = PSigDisposition_CORE_DUMP
   , [PSignal_SIGKILL]   = PSigDisposition_TERMINATE
   , [PSignal_SIGUSR1]   = PSigDisposition_TERMINATE
   , [PSignal_SIGSEGV]   = PSigDisposition_CORE_DUMP
   , [PSignal_SIGUSR2]   = PSigDisposition_TERMINATE
   , [PSignal_SIGPIPE]   = PSigDisposition_TERMINATE
   , [PSignal_SIGALRM]   = PSigDisposition_TERMINATE
   , [PSignal_SIGTERM]   = PSigDisposition_TERMINATE
   , [PSignal_SIGSTKFLT] = PSigDisposition_TERMINATE
   , [PSignal_SIGCHLD]   = PSigDisposition_CONTINUE
   , [PSignal_SIGCONT]   = PSigDisposition_CONTINUE
   , [PSignal_SIGSTOP]   = PSigDisposition_STOP
   , [PSignal_SIGTSTP]   = PSigDisposition_STOP
   , [PSignal_SIGTTIN]   = PSigDisposition_STOP
   , [PSignal_SIGTTOU]   = PSigDisposition_STOP
   , [PSignal_SIGURG]    = PSigDisposition_IGNORE
   , [PSignal_SIGXCPU]   = PSigDisposition_CORE_DUMP
   , [PSignal_SIGXFSZ]   = PSigDisposition_CORE_DUMP
   , [PSignal_SIGVTALRM] = PSigDisposition_TERMINATE
   , [PSignal_SIGPROF]   = PSigDisposition_TERMINATE
   , [PSignal_SIGWINCH]  = PSigDisposition_IGNORE
   , [PSignal_SIGIO]     = PSigDisposition_TERMINATE
   , [PSignal_SIGPWR]    = PSigDisposition_TERMINATE
   , [PSignal_SIGSYS]    = PSigDisposition_CORE_DUMP
};

static_assert(arrayCapacity(S_PSIG_STD_DISPS) == PSignal_ENUM_STD_COUNT);


//================================================================================================
// Public API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Validation
//------------------------------------------------------------------------------------------------

bool psignal_disposition_validate(unsigned const v)
{
   // The code below assumes that we start from 0 and enum is unsigned.
   static_assert(PSigDisposition_ENUM_FIRST == 0 && (PSigDisposition)-1 > 0);

   return v < PSigDisposition_ENUM_COUNT;
}


//------------------------------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------------------------------

PSigDisposition psignal_disposition_default(PSignal const psig)
{
   return psignal_is_standard(psig) ? S_PSIG_STD_DISPS[psig] : PSigDisposition_UNSPECIFIED;
}

ascii const *psignal_disposition_desc(PSigDisposition const pdisp)
{
   switch (pdisp)
   {
      case PSigDisposition_CONTINUE:    return "Continue";
      case PSigDisposition_CORE_DUMP:   return "Core Dump";
      case PSigDisposition_IGNORE:      return "Ignore";
      case PSigDisposition_STOP:        return "Stop";
      case PSigDisposition_TERMINATE:   return "Terminate";
      case PSigDisposition_UNSPECIFIED: return "Unspecified";
   }

   unreachable();
}
