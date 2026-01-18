#include "libposix_signals/posix_signal_dispositions.h"

#include <stddef.h>


//================================================================================================
// Internal Data
//================================================================================================


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

PSigDisposition psignal_disposition_default(PSignal)
{
   // TODO
   return PSigDisposition_UNSPECIFIED;
}

PSigDisposition psignal_disposition_current(PSignal)
{
   // TODO
   return PSigDisposition_UNSPECIFIED;   
}

bool psignal_disposition_override(PSignal, PSigDisposition)
{
   // TODO
   return false;
}

void psignal_disposition_reset(PSignal)
{
   // TODO
}

ascii const *psignal_disposition_desc(PSignal const psig)
{
   PSigDisposition const disp = psignal_disposition_current(psig);

   switch (disp)
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
