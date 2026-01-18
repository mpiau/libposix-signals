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

typedef enum PSigDisposition : unsigned char
{
     PSigDisposition_CONTINUE    // Continue the process if it's currently stopped.
   , PSigDisposition_CORE_DUMP   // Terminate the process and produces core dump file.
   , PSigDisposition_IGNORE      // Ignore the signal.
   , PSigDisposition_STOP        // Stop the process.
   , PSigDisposition_TERMINATE   // Terminate the process.
   , PSigDisposition_UNSPECIFIED // Not specified by POSIX, OS specific.
} PSigDisposition;

static constexpr unsigned PSigDisposition_ENUM_FIRST = PSigDisposition_CONTINUE;
static constexpr unsigned PSigDisposition_ENUM_LAST  = PSigDisposition_UNSPECIFIED;
static constexpr unsigned PSigDisposition_ENUM_COUNT = (PSigDisposition_ENUM_LAST - PSigDisposition_ENUM_FIRST) + 1;


typedef char ascii;


//================================================================================================
// Public API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Validation
//------------------------------------------------------------------------------------------------

/*
   Validates that the given value belongs to PSigDisposition.
   All the functions taking a PSigDisposition WILL ASSUME that the given enum value is VALID.
*/
[[nodiscard]]
bool psignal_disposition_validate(unsigned);


//------------------------------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------------------------------

/*
   Returns the default disposition defined for the given signal.
*/
[[nodiscard]]
PSigDisposition psignal_disposition_default(PSignal);

/*
   Returns the disposition currently applied for the given signal.
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
   Reset the disposition of the given signal to its default.
*/
void psignal_disposition_reset(PSignal);

/*
   Returns a simple description for the current disposition. Useful for logging/debugging.
   Example: "Terminate" will be returned for PSigDisposition_TERMINATE.
*/
[[nodiscard]]
ascii const *psignal_disposition_desc(PSignal);
