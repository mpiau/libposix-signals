#include "libposix_signals/posix_signal_emission_reason.h"

#define _GNU_SOURCE
#include <signal.h>


// ===============================================================================================
// Internal Data
// ===============================================================================================

static constexpr char UNKNOWN_REASON[] = "Unknown reason";


// ===============================================================================================
// Internal functions
// ===============================================================================================

static char const *sigill_signal_reason(int const code)
{
   switch (code)
   {
      case ILL_ILLOPC:   return "Illegal opcode.";
      case ILL_ILLOPN:   return "Illegal operand.";
      case ILL_ILLADR:   return "Illegal addressing mode.";
      case ILL_ILLTRP:   return "Illegal trap.";
      case ILL_PRVOPC:   return "Privileged opcode.";
      case ILL_PRVREG:   return "Privileged register.";
      case ILL_COPROC:   return "Coprocessor error.";
      case ILL_BADSTK:   return "Internal stack error.";
      case ILL_BADIADDR: return "Unimplemented instruction address";

      default: return UNKNOWN_REASON;
   }
}

static char const *sigfpe_signal_reason(int const code)
{
   switch (code)
   {
      case FPE_INTDIV:   return "Integer divide by zero.";
      case FPE_INTOVF:   return "Integer overflow.";
      case FPE_FLTDIV:   return "Floating-point divide by zero.";
      case FPE_FLTOVF:   return "Floating-point overflow.";
      case FPE_FLTUND:   return "Floating-point underflow.";
      case FPE_FLTRES:   return "Floating-point inexact result.";
      case FPE_FLTINV:   return "Floating-point invalid operation.";
      case FPE_FLTSUB:   return "Subscript out of range.";
      case FPE_FLTUNK:   return "Undiagnosed floating-point exception";
      case FPE_CONDTRAP: return "Trap on condition.";

      default: return UNKNOWN_REASON;
   }
}

static char const *sigsegv_signal_reason(int const code)
{
   switch (code)
   {
      case SEGV_MAPERR:  return "Address not mapped to object.";
      case SEGV_ACCERR:  return "Invalid permissions for mapped object.";
      case SEGV_BNDERR:  return "Bounds checking failure.";
      case SEGV_PKUERR:  return "Protection key checking failure.";
      case SEGV_ACCADI:  return "ADI not enabled for mapped object.";
      case SEGV_ADIDERR: return "Disrupting MCD Error.";
      case SEGV_ADIPERR: return "Precise MCD exception.";
      case SEGV_MTEAERR: return "Asynchronous ARM MTE error.";
      case SEGV_MTESERR: return "Synchronous ARM MTE exception.";
      case SEGV_CPERR:   return "Control protection fault.";

      default: return UNKNOWN_REASON;
   }
}

static char const *sigbus_signal_reason(int const code)
{
   switch (code)
   {
      case BUS_ADRALN:    return "Invalid address alignement.";
      case BUS_ADRERR:    return "Non-existent physical address.";
      case BUS_OBJERR:    return "Object specific hardware error.";
      case BUS_MCEERR_AR: return "Hardware memory error: action required.";
      case BUS_MCEERR_AO: return "Hardware memory error: action optional.";

      default: return UNKNOWN_REASON;
   }
}

static char const *sigtrap_signal_reason(int const code)
{
   switch (code)
   {
      case TRAP_BRKPT:  return "Process breakpoint.";
      case TRAP_TRACE:  return "Process trace trap.";
      case TRAP_BRANCH: return "Process taken branch trap.";
      case TRAP_HWBKPT: return "Hardware breakpoint/watchpoint.";
      case TRAP_UNK:    return "Undiagnosed trap.";

      default: return UNKNOWN_REASON;
   }
}

static char const *sigchld_signal_reason(int const code)
{
   switch (code)
   {
      case CLD_EXITED:    return "Child has exited.";
      case CLD_KILLED:    return "Child was killed.";
      case CLD_DUMPED:    return "Child terminated abnormally.";
      case CLD_TRAPPED:   return "Traced child has trapped.";
      case CLD_STOPPED:   return "Child has stopped.";
      case CLD_CONTINUED: return "Stopped child has continued";

      default: return UNKNOWN_REASON;
   }
}

static char const *sigpoll_signal_reason(int const code)
{
   switch (code)
   {
      case POLL_IN:  return "Data input available.";
      case POLL_OUT: return "Output buffers available.";
      case POLL_MSG: return "Input message available.";
      case POLL_ERR: return "I/O error.";
      case POLL_PRI: return "High priority input available.";
      case POLL_HUP: return "Device disconnected.";

      default: return UNKNOWN_REASON;
   }
}

static char const *other_signals_reason(int const code)
{
   switch (code)
   {
      case SI_ASYNCNL:  return "Sent by asynch name lookup completion.";
      case SI_DETHREAD: return "Sent by execve killing subsidiary threads.";
      case SI_TKILL:    return "Sent by tkill.";
      case SI_SIGIO:    return "Sent by queued SIGIO.";
      case SI_ASYNCIO:  return "Sent by AIO completion.";
      case SI_MESGQ:    return "Sent by real-time mesq state change.";
      case SI_TIMER:    return "Sent by timer expiration.";
      case SI_QUEUE:    return "Sent by sigqueue.";
      case SI_USER:     return "Sent by kill, sigsend.";
      case SI_KERNEL:   return "Sent by kernel.";

      default: return UNKNOWN_REASON;
   }
}


// ===============================================================================================
// Public functions
// ===============================================================================================

char const *sig_emission_reason(PSignal const sig, int const code)
{
   switch(sig)
   {
      case PSignal_SIGILL:  return sigill_signal_reason(code);
      case PSignal_SIGFPE:  return sigfpe_signal_reason(code);
      case PSignal_SIGSEGV: return sigsegv_signal_reason(code);
      case PSignal_SIGBUS:  return sigbus_signal_reason(code);
      case PSignal_SIGTRAP: return sigtrap_signal_reason(code);
      case PSignal_SIGCHLD: return sigchld_signal_reason(code);
      case PSignal_SIGPOLL: return sigpoll_signal_reason(code);

      default: return other_signals_reason(code);
   }
}
