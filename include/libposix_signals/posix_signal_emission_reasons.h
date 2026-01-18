#pragma once

#include "posix_signals.h"


//================================================================================================
// POSIX Signal Emission reasons
//================================================================================================

/* 
   When a signal is delivered to the current process, there is often an underlying reason for it.
   This information helps answer the question: “Why did I receive this signal?”

   If you are implementing a crash handler or logging system, displaying this extra-information
   can be extremely valuable.
*/

typedef int  sigcode;
typedef char ascii;


//================================================================================================
// API Functions
//================================================================================================


/*
   Returns a non-null string containing the underlying reason of why the signal has been received.
   If you are familiar with siginfo_t, sigcode is the value extracted from si_code.
   If no particular reason is found, a simple "Unspecified reason" will be returned.
*/
[[nodiscard]]
ascii const *psig_emission_reason(PSignal, sigcode);
