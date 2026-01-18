#pragma once

#include "posix_signals.h"

//================================================================================================
// POSIX Signal Dispositions
//================================================================================================

/* 
   Each signal has a default disposition, which specify how the process should react when that
   signal is delivered. (you could translate that to default "action").

   For the standard (non RT) signals, these default dispositions are clearly defined by POSIX.
   However, there is no standardized default behaviour for Real-Time signals.
   As such, this library can't assume a particular disposition and will mark all real-time signals
   as PSigDisposition_UNSPECIFIED.

   You can modify the disposition of both STD and RT signals (except for a few exceptions).
*/


enum PSigDisposition : unsigned char
{
     PSigDisposition_CONTINUE    // Continue the process if it's currently stopped.
   , PSigDisposition_CORE_DUMP   // Terminate the process and produces core dump file.
   , PSigDisposition_IGNORE      // Ignore the signal.
   , PSigDisposition_STOP        // Stop the process.
   , PSigDisposition_TERMINATE   // Terminate the process.
   , PSigDisposition_UNSPECIFIED // Not specified by POSIX, OS specific.

   //---------------------------------------------------------------------------------------------
   // Helpers
   //---------------------------------------------------------------------------------------------

   , PSigDisposition_EnumFirst = PSigDisposition_CONTINUE
   , PSigDisposition_EnumLast  = PSigDisposition_UNSPECIFIED
   , PSigDisposition_EnumCount = (PSigDisposition_EnumLast - PSigDisposition_EnumFirst) + 1
};

typedef enum PSigDisposition PSigDisposition;


//================================================================================================
// API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Validation
//------------------------------------------------------------------------------------------------

/*
   Validates that the given value belongs to PSigDisposition.
*/
[[nodiscard]]
bool psignal_disposition_validate(unsigned char);


//------------------------------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------------------------------

/*
   Returns the default disposition defined for the given signal.
*/
[[nodiscard]]
PSigDisposition psignal_disposition_default(PSignal);

/*
   Returns the current disposition applied for the given signal.
*/
[[nodiscard]]
PSigDisposition psignal_disposition_current(PSignal);

/*
   Override the disposition of the given signal by the given disposition.
   - You can't override the disposition of PSignal_SIGKILL and PSignal_SIGSTOP.
   - You can't set the disposition to PSigDisposition_UNSPECIFIED.
   - True will be returned if the requested disposition is already in used.
*/
[[nodiscard]]
bool psignal_disposition_override(PSignal, PSigDisposition);

/*
   Returns the current disposition applied for the given signal.
   - PSignal is assumed to be valid.
*/
void psignal_disposition_reset(PSignal);

/*
   Returns a simple description for the current disposition.
   Useful for logging/debugging.
   - PSignal is assumed to be valid.
   Example: "Terminate" will be returned for PSigDisposition_TERMINATE.
*/
[[nodiscard]] char const *psignal_disposition_desc(PSignal);
