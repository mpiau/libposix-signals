#pragma once

#include "posix_signals.h"


//================================================================================================
// POSIX Signal Bitmasks
//================================================================================================

/*
   These compile-time bitmasks have been made to simplify and optimize playing with one or
   multiple signals at the same time.
   Each PSignal is associated to a specific bit following the rule: (1ul << PSignal).
*/

typedef unsigned _BitInt(PSignal_Count) PSignalMask;


//================================================================================================
// POSIX Signal Bitmask Definitions
//================================================================================================

//------------------------------------------------------------------------------------------------
// POSIX Signal Type Bitmasks
//------------------------------------------------------------------------------------------------

/*
   Bitmask representing all defined STANDARD POSIX Signals.
*/
static constexpr PSignalMask PSignalMask_STANDARD_SIGNALS
   = (1ul << PSignal_SIGHUP)      | (1ul << PSignal_SIGINT)      | (1ul << PSignal_SIGQUIT)
   | (1ul << PSignal_SIGILL)      | (1ul << PSignal_SIGTRAP)     | (1ul << PSignal_SIGABRT)
   | (1ul << PSignal_SIGBUS)      | (1ul << PSignal_SIGFPE)      | (1ul << PSignal_SIGKILL)
   | (1ul << PSignal_SIGUSR1)     | (1ul << PSignal_SIGSEGV)     | (1ul << PSignal_SIGUSR2)
   | (1ul << PSignal_SIGPIPE)     | (1ul << PSignal_SIGALRM)     | (1ul << PSignal_SIGTERM)
   | (1ul << PSignal_SIGSTKFLT)   | (1ul << PSignal_SIGCHLD)     | (1ul << PSignal_SIGCONT)
   | (1ul << PSignal_SIGSTOP)     | (1ul << PSignal_SIGTSTP)     | (1ul << PSignal_SIGTTIN)
   | (1ul << PSignal_SIGTTOU)     | (1ul << PSignal_SIGURG)      | (1ul << PSignal_SIGXCPU)
   | (1ul << PSignal_SIGXFSZ)     | (1ul << PSignal_SIGVTALRM)   | (1ul << PSignal_SIGPROF)
   | (1ul << PSignal_SIGWINCH)    | (1ul << PSignal_SIGIO)       | (1ul << PSignal_SIGPWR)
   | (1ul << PSignal_SIGSYS);

/*
   Bitmask representing all defined REAL-TIME POSIX Signals.
*/
static constexpr PSignalMask PSignalMask_REALTIME_SIGNALS
   = (1ul << PSignal_SIGRTMIN)    | (1ul << PSignal_SIGRTMIN_1)  | (1ul << PSignal_SIGRTMIN_2)
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


//------------------------------------------------------------------------------------------------
// POSIX Signal Disposition Bitmasks
//------------------------------------------------------------------------------------------------

/* 
   Each signal has a default disposition, which specify how the process should react when that
   signal is delivered. (you could translate that to default "action").

   For the standard (non RT) signals, these default dispositions are clearly defined by POSIX.
   However, there is no standardized default behaviour for Real-Time signals.
   As such, this library can't assume a particular disposition and will mark all real-time signals
   as unspecified.
*/

/*
   Bitmask representing all POSIX signals whose default disposition is to
   produce a core dump (crash file) and terminate the process execution.
*/
static constexpr PSignalMask PSignalMask_DISPOSITION_CORE_DUMP
   = (1ul << PSignal_SIGQUIT)   | (1ul << PSignal_SIGILL)  | (1ul << PSignal_SIGTRAP)
   | (1ul << PSignal_SIGABRT)   | (1ul << PSignal_SIGBUS)  | (1ul << PSignal_SIGFPE)
   | (1ul << PSignal_SIGSEGV)   | (1ul << PSignal_SIGXCPU) | (1ul << PSignal_SIGXFSZ)
   | (1ul << PSignal_SIGSYS);

/*
   Bitmask representing all POSIX signals whose default disposition is to
   terminate the process.
*/
static constexpr PSignalMask PSignalMask_DISPOSITION_TERMINATE
   = (1ul << PSignal_SIGHUP)    | (1ul << PSignal_SIGINT)  | (1ul << PSignal_SIGKILL)
   | (1ul << PSignal_SIGUSR1)   | (1ul << PSignal_SIGUSR2) | (1ul << PSignal_SIGPIPE)
   | (1ul << PSignal_SIGALRM)   | (1ul << PSignal_SIGTERM) | (1ul << PSignal_SIGSTKFLT)
   | (1ul << PSignal_SIGVTALRM) | (1ul << PSignal_SIGPROF) | (1ul << PSignal_SIGIO)
   | (1ul << PSignal_SIGPWR);

/*
   Bitmask representing all POSIX signals whose default disposition is to be
   ignored by the process.
*/
static constexpr PSignalMask PSignalMask_DISPOSITION_IGNORE
   = (1ul << PSignal_SIGURG)    | (1ul << PSignal_SIGWINCH);

/*
   Bitmask representing all POSIX signals whose default disposition is to
   pause/suspend the process.
*/
static constexpr PSignalMask PSignalMask_DISPOSITION_STOP
   = (1ul << PSignal_SIGSTOP)   | (1ul << PSignal_SIGTSTP) | (1ul << PSignal_SIGTTIN)
   | (1ul << PSignal_SIGTTOU);

/*
   Bitmask representing all POSIX signals whose default disposition is to
   resume the execution of a paused/suspended process
*/
static constexpr PSignalMask PSignalMask_DISPOSITION_CONTINUE
   = (1ul << PSignal_SIGCHLD) | (1ul << PSignal_SIGCONT);

/*
   Bitmask representing all POSIX signals whose default disposition is not
   specified by the POSIX standard and thus not predictable.
*/
static constexpr PSignalMask PSignalMask_DISPOSITION_UNSPECIFIED
   = PSignalMask_REALTIME_SIGNALS;


//------------------------------------------------------------------------------------------------
// POSIX Signal Helpers Bitmasks
//------------------------------------------------------------------------------------------------

/*
   Bitmask representing no signal and that can be used as a default/reset value.
*/
static constexpr PSignalMask PSignalMask_NONE
   = 0ul;

/*
   Bitmask representing all defined POSIX Signals.
*/
static constexpr PSignalMask PSignalMask_ALL
   = PSignalMask_STANDARD_SIGNALS
   | PSignalMask_REALTIME_SIGNALS;

/*
   Bitmask representing all POSIX signals considered as fatal by the process
   when received and will end its execution.
*/
static constexpr PSignalMask PSignalMask_FATAL_SIGNALS
   = PSignalMask_DISPOSITION_CORE_DUMP
   | PSignalMask_DISPOSITION_TERMINATE;

/*
   Bitmask representing all POSIX signals that can't be hooked according to the
   POSIX standard.
*/
static constexpr PSignalMask PSignalMask_UNHOOKABLE_SIGNALS
   = (1ul << PSignal_SIGKILL) | (1ul << PSignal_SIGSTOP);

/*
   Bitmask representing all POSIX signals that can be hooked on according to the
   POSIX standard.
*/
static constexpr PSignalMask PSignalMask_HOOKABLE_SIGNALS
   = PSignalMask_ALL
   ^ PSignalMask_UNHOOKABLE_SIGNALS;


//================================================================================================
// Compile-Time Invariants
//================================================================================================

static_assert(
   sizeof(PSignalMask) <= 64u, "Binary mask must not exceed 64 bits in size."
);

static_assert(
   (PSignalMask_ALL
      ^ PSignalMask_STANDARD_SIGNALS
      ^ PSignalMask_REALTIME_SIGNALS
   ) == PSignalMask_NONE, "All signals must be defined as either STANDARD or REAL-TIME."
);

static_assert(
   (PSignalMask_ALL
      ^ PSignalMask_DISPOSITION_CORE_DUMP
      ^ PSignalMask_DISPOSITION_STOP
      ^ PSignalMask_DISPOSITION_TERMINATE
      ^ PSignalMask_DISPOSITION_CONTINUE
      ^ PSignalMask_DISPOSITION_IGNORE
      ^ PSignalMask_DISPOSITION_UNSPECIFIED
   ) == PSignalMask_NONE, "All signals must define exactly one default disposition."
);

static_assert(
   __builtin_popcountll(PSignalMask_STANDARD_SIGNALS) == PSignal_StandardCount,
   "That bitmask must represent all standard defined signals."
);

static_assert(
   __builtin_popcountll(PSignalMask_REALTIME_SIGNALS) == PSignal_RealtimeCount,
   "That bitmask must represent all real-time defined signals."
);

static_assert(
   __builtin_popcountll(PSignalMask_ALL) == PSignal_Count,
   "That bitmask must represent all defined signals."
);
