#include "libposix_signals/posix_signal_dispositions.h"
#include "libposix_signals/posix_signals.h"
#include "libmacros/macro_utils.h"

#include <stddef.h>
#include <stdint.h>


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
static_assert(array_capacity(S_PSIG_STD_DISPS) == PSignal_ENUM_STD_COUNT);

static constexpr PSignalMask PSigDispositionMask_CONTINUE =
     (1ul << PSignal_SIGCHLD) | (1ul << PSignal_SIGCONT);

static constexpr PSignalMask PSigDispositionMask_CORE_DUMP =
     (1ul << PSignal_SIGQUIT) | (1ul << PSignal_SIGILL)  | (1ul << PSignal_SIGTRAP)
   | (1ul << PSignal_SIGABRT) | (1ul << PSignal_SIGBUS)  | (1ul << PSignal_SIGFPE)
   | (1ul << PSignal_SIGSEGV) | (1ul << PSignal_SIGXCPU) | (1ul << PSignal_SIGXFSZ)
   | (1ul << PSignal_SIGSYS);

static constexpr PSignalMask PSigDispositionMask_IGNORE =
     (1ul << PSignal_SIGURG) | (1ul << PSignal_SIGWINCH);

static constexpr PSignalMask PSigDispositionMask_STOP =
     (1ul << PSignal_SIGSTOP) | (1ul << PSignal_SIGTSTP) | (1ul << PSignal_SIGTTIN)
   | (1ul << PSignal_SIGTTOU);

static constexpr PSignalMask PSigDispositionMask_TERMINATE = 
     (1ul << PSignal_SIGHUP)    | (1ul << PSignal_SIGINT)  | (1ul << PSignal_SIGKILL)
   | (1ul << PSignal_SIGUSR1)   | (1ul << PSignal_SIGUSR2) | (1ul << PSignal_SIGPIPE)
   | (1ul << PSignal_SIGALRM)   | (1ul << PSignal_SIGTERM) | (1ul << PSignal_SIGSTKFLT)
   | (1ul << PSignal_SIGVTALRM) | (1ul << PSignal_SIGPROF) | (1ul << PSignal_SIGIO)
   | (1ul << PSignal_SIGPWR);

static constexpr PSignalMask PSigDispositionMask_UNSPECIFIED =
     (1ul << PSignal_SIGRTMIN )   | (1ul << PSignal_SIGRTMIN_1)  | (1ul << PSignal_SIGRTMIN_2)
   | (1ul << PSignal_SIGRTMIN_3)  | (1ul << PSignal_SIGRTMIN_4)  | (1ul << PSignal_SIGRTMIN_5)
   | (1ul << PSignal_SIGRTMIN_6)  | (1ul << PSignal_SIGRTMIN_7)  | (1ul << PSignal_SIGRTMIN_8)
   | (1ul << PSignal_SIGRTMIN_9)  | (1ul << PSignal_SIGRTMIN_10) | (1ul << PSignal_SIGRTMIN_11)
   | (1ul << PSignal_SIGRTMIN_12) | (1ul << PSignal_SIGRTMIN_13) | (1ul << PSignal_SIGRTMIN_14)
   | (1ul << PSignal_SIGRTMIN_15) | (1ul << PSignal_SIGRTMAX_14) | (1ul << PSignal_SIGRTMAX_13)
   | (1ul << PSignal_SIGRTMAX_12) | (1ul << PSignal_SIGRTMAX_11) | (1ul << PSignal_SIGRTMAX_10)
   | (1ul << PSignal_SIGRTMAX_9)  | (1ul << PSignal_SIGRTMAX_8)  | (1ul << PSignal_SIGRTMAX_7)
   | (1ul << PSignal_SIGRTMAX_6)  | (1ul << PSignal_SIGRTMAX_5)  | (1ul << PSignal_SIGRTMAX_4)
   | (1ul << PSignal_SIGRTMAX_3)  | (1ul << PSignal_SIGRTMAX_2)  | (1ul << PSignal_SIGRTMAX_1)
   | (1ul << PSignal_SIGRTMAX);


static constexpr PSignalMask PSigDispositionMask_NONE = 0ul;
static constexpr PSignalMask PSigDispositionMask_ALL =
     PSigDispositionMask_CONTINUE  | PSigDispositionMask_CORE_DUMP
   | PSigDispositionMask_IGNORE    | PSigDispositionMask_STOP
   | PSigDispositionMask_TERMINATE | PSigDispositionMask_UNSPECIFIED;

static_assert(PSigDispositionMask_ALL == 0b00111111'11111111'11111111'11111111'11111111'11111111'11111111'11111111);


//================================================================================================
// Public API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Validation
//------------------------------------------------------------------------------------------------

bool psignal_disposition_validate(unsigned const v)
{
   // The code below assumes that we start from 0 and enum is unsigned.
   static_assert(PSigDisposition_ENUM_FIRST == 0 && type_is_unsigned(PSigDisposition));

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

PSignalMask psignal_disposition_mask(PSigDisposition const pdisp)
{
   switch (pdisp)
   {
      case PSigDisposition_CONTINUE:    return PSigDispositionMask_CONTINUE;
      case PSigDisposition_CORE_DUMP:   return PSigDispositionMask_CORE_DUMP;
      case PSigDisposition_IGNORE:      return PSigDispositionMask_IGNORE;
      case PSigDisposition_STOP:        return PSigDispositionMask_STOP;
      case PSigDisposition_TERMINATE:   return PSigDispositionMask_TERMINATE;
      case PSigDisposition_UNSPECIFIED: return PSigDispositionMask_UNSPECIFIED;
   }

   unreachable();
}

PSignalMask psignal_disposition_mask_all(void)
{
   return PSigDispositionMask_ALL;
}

PSignalMask psignal_disposition_mask_none(void)
{
   return PSigDispositionMask_NONE;
}