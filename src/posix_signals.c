#include "libposix_signals/posix_signals.h"
#include "libposix_signals/posix_signal_dispositions.h"
#include "libmacros/macro_utils.h"

#include <assert.h>
#include <signal.h>


//================================================================================================
// Private Data
//================================================================================================

struct StdSigProperties
{
   int rawSignal;
   char const *name;
   char const *desc;
};
typedef struct StdSigProperties StdSigProperties;

struct RTSigProperties
{
   char const *name;
   char const *desc;
};
typedef struct RTSigProperties RTSigProperties;


#define CG_STDSIG_PROPS(pSig, pRaw, pDesc)  \
   [pSig] = (StdSigProperties) { \
      .rawSignal = pRaw,         \
      .name = #pRaw,             \
      .desc = pDesc              \
   }
static constexpr StdSigProperties S_STD_SIGNALS_PROPS[] =
{
     CG_STDSIG_PROPS(PSignal_SIGHUP,    SIGHUP,    "Terminal Hang-Up / Process Death Detected")
   , CG_STDSIG_PROPS(PSignal_SIGINT,    SIGINT,    "User Interrupt (Ctrl+C)")
   , CG_STDSIG_PROPS(PSignal_SIGQUIT,   SIGQUIT,   "Quit from keyboard")
   , CG_STDSIG_PROPS(PSignal_SIGILL,    SIGILL,    "Illegal Instruction")
   , CG_STDSIG_PROPS(PSignal_SIGTRAP,   SIGTRAP,   "Trace / Breakpoint trap")
   , CG_STDSIG_PROPS(PSignal_SIGABRT,   SIGABRT,   "Abort signal")
   , CG_STDSIG_PROPS(PSignal_SIGBUS,    SIGBUS,    "Bus error (bad memory access)")
   , CG_STDSIG_PROPS(PSignal_SIGFPE,    SIGFPE,    "Erroneous arithmetic operation")
   , CG_STDSIG_PROPS(PSignal_SIGKILL,   SIGKILL,   "Kill signal")
   , CG_STDSIG_PROPS(PSignal_SIGUSR1,   SIGUSR1,   "User-defined signal 1")
   , CG_STDSIG_PROPS(PSignal_SIGSEGV,   SIGSEGV,   "Invalid memory reference (Segmentation Fault)")
   , CG_STDSIG_PROPS(PSignal_SIGUSR2,   SIGUSR2,   "User-defined signal 2")
   , CG_STDSIG_PROPS(PSignal_SIGPIPE,   SIGPIPE,   "Broken pipe: write to pipe with no readers")
   , CG_STDSIG_PROPS(PSignal_SIGALRM,   SIGALRM,   "Timer signal")
   , CG_STDSIG_PROPS(PSignal_SIGTERM,   SIGTERM,   "Termination signal")
   , CG_STDSIG_PROPS(PSignal_SIGSTKFLT, SIGSTKFLT, "Stack fault on coprocessor")
   , CG_STDSIG_PROPS(PSignal_SIGCHLD,   SIGCHLD,   "Child stopped, terminated, or continued")
   , CG_STDSIG_PROPS(PSignal_SIGCONT,   SIGCONT,   "Continue if stopped")
   , CG_STDSIG_PROPS(PSignal_SIGSTOP,   SIGSTOP,   "Stop process")
   , CG_STDSIG_PROPS(PSignal_SIGTSTP,   SIGTSTP,   "Stop typed at terminal")
   , CG_STDSIG_PROPS(PSignal_SIGTTIN,   SIGTTIN,   "Terminal input for background process")
   , CG_STDSIG_PROPS(PSignal_SIGTTOU,   SIGTTOU,   "Terminal output for background process")
   , CG_STDSIG_PROPS(PSignal_SIGURG,    SIGURG,    "Urgent condition on socket")
   , CG_STDSIG_PROPS(PSignal_SIGXCPU,   SIGXCPU,   "CPU time limit exceeded")
   , CG_STDSIG_PROPS(PSignal_SIGXFSZ,   SIGXFSZ,   "File size limit exceeded")
   , CG_STDSIG_PROPS(PSignal_SIGVTALRM, SIGVTALRM, "Virtual alarm clock")
   , CG_STDSIG_PROPS(PSignal_SIGPROF,   SIGPROF,   "Profiling timer expired")
   , CG_STDSIG_PROPS(PSignal_SIGWINCH,  SIGWINCH,  "Window resize signal")
   , CG_STDSIG_PROPS(PSignal_SIGIO,     SIGIO,     "I/O now possible")
   , CG_STDSIG_PROPS(PSignal_SIGPWR,    SIGPWR,    "Power failure (System V)")
   , CG_STDSIG_PROPS(PSignal_SIGSYS,    SIGSYS,    "Bad system call (SVr4)")
};
#undef CG_STDSIG_PROPS

static constexpr RTSigProperties S_RT_SIGNALS_PROPS[] =
{
     (RTSigProperties) { .name = "SIGRTMIN",      .desc = "Real-time signal 0" }
   , (RTSigProperties) { .name = "SIGRTMIN + 1",  .desc = "Real-time signal 1" }
   , (RTSigProperties) { .name = "SIGRTMIN + 2",  .desc = "Real-time signal 2" }
   , (RTSigProperties) { .name = "SIGRTMIN + 3",  .desc = "Real-time signal 3" }
   , (RTSigProperties) { .name = "SIGRTMIN + 4",  .desc = "Real-time signal 4" }
   , (RTSigProperties) { .name = "SIGRTMIN + 5",  .desc = "Real-time signal 5" }
   , (RTSigProperties) { .name = "SIGRTMIN + 6",  .desc = "Real-time signal 6" }
   , (RTSigProperties) { .name = "SIGRTMIN + 7",  .desc = "Real-time signal 7" }
   , (RTSigProperties) { .name = "SIGRTMIN + 8",  .desc = "Real-time signal 8" }
   , (RTSigProperties) { .name = "SIGRTMIN + 9",  .desc = "Real-time signal 9" }
   , (RTSigProperties) { .name = "SIGRTMIN + 10", .desc = "Real-time signal 10" }
   , (RTSigProperties) { .name = "SIGRTMIN + 11", .desc = "Real-time signal 11" }
   , (RTSigProperties) { .name = "SIGRTMIN + 12", .desc = "Real-time signal 12" }
   , (RTSigProperties) { .name = "SIGRTMIN + 13", .desc = "Real-time signal 13" }
   , (RTSigProperties) { .name = "SIGRTMIN + 14", .desc = "Real-time signal 14" }
   , (RTSigProperties) { .name = "SIGRTMIN + 15", .desc = "Real-time signal 15" }

   , (RTSigProperties) { .name = "SIGRTMAX - 14", .desc = "Real-time signal 16" }
   , (RTSigProperties) { .name = "SIGRTMAX - 13", .desc = "Real-time signal 17" }
   , (RTSigProperties) { .name = "SIGRTMAX - 12", .desc = "Real-time signal 18" }
   , (RTSigProperties) { .name = "SIGRTMAX - 11", .desc = "Real-time signal 19" }
   , (RTSigProperties) { .name = "SIGRTMAX - 10", .desc = "Real-time signal 20" }
   , (RTSigProperties) { .name = "SIGRTMAX - 9",  .desc = "Real-time signal 21" }
   , (RTSigProperties) { .name = "SIGRTMAX - 8",  .desc = "Real-time signal 22" }
   , (RTSigProperties) { .name = "SIGRTMAX - 7",  .desc = "Real-time signal 23" }
   , (RTSigProperties) { .name = "SIGRTMAX - 6",  .desc = "Real-time signal 24" }
   , (RTSigProperties) { .name = "SIGRTMAX - 5",  .desc = "Real-time signal 25" }
   , (RTSigProperties) { .name = "SIGRTMAX - 4",  .desc = "Real-time signal 26" }
   , (RTSigProperties) { .name = "SIGRTMAX - 3",  .desc = "Real-time signal 27" }
   , (RTSigProperties) { .name = "SIGRTMAX - 2",  .desc = "Real-time signal 28" }
   , (RTSigProperties) { .name = "SIGRTMAX - 1",  .desc = "Real-time signal 29" }
   , (RTSigProperties) { .name = "SIGRTMAX",      .desc = "Real-time signal 30" }
};


static_assert(array_capacity(S_STD_SIGNALS_PROPS) == PSignal_ENUM_STD_COUNT);
static_assert(array_capacity(S_RT_SIGNALS_PROPS)  == PSignal_ENUM_RT_COUNT);


//================================================================================================
// Private Functions
//================================================================================================

// We can't assume that STD/RT signals are 0-indexed so these functions are defined for that.

[[nodiscard]] static inline
unsigned std_sig_idx(PSignal const psig)
{
   assert(psignal_is_standard(psig));
   return (psig - PSignal_ENUM_STD_FIRST);
}

[[nodiscard]] static inline
unsigned rt_sig_idx(PSignal const psig)
{
   assert(psignal_is_real_time(psig));
   return (psig - PSignal_ENUM_RT_FIRST);
}


//================================================================================================
// Public API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Identification
//------------------------------------------------------------------------------------------------

bool psignal_validate(unsigned const v)
{
   // The code below assumes that we start from 0 and enum is unsigned.
   static_assert(PSignal_ENUM_STD_FIRST == 0 && (PSignal)-1 > 0);

   return v <= PSignal_ENUM_LAST;
}


//------------------------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------------------------

bool psignal_is_standard(PSignal const psig)
{
   // The code below assumes that we start from 0 and enum is unsigned.
   static_assert(PSignal_ENUM_STD_FIRST == 0 && (PSignal)-1 > 0);

   return psig <= PSignal_ENUM_STD_LAST;
}

bool psignal_is_real_time(PSignal const psig)
{
   return !(psig < PSignal_ENUM_RT_FIRST || psig > PSignal_ENUM_RT_LAST);
}

int psignal_to_raw_signal(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[std_sig_idx(psig)].rawSignal
      : SIGRTMIN + (int)rt_sig_idx(psig);
}

char const *psignal_name(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[std_sig_idx(psig)].name
      : S_RT_SIGNALS_PROPS[rt_sig_idx(psig)].name;
}

char const *psignal_desc(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[std_sig_idx(psig)].desc
      : S_RT_SIGNALS_PROPS[rt_sig_idx(psig)].desc;
}


//------------------------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------------------------

bool psignal_from_raw_signal(int const signal, PSignal *const out)
{
   // STD Signals
   for (PSignal idx = PSignal_ENUM_STD_FIRST; idx <= PSignal_ENUM_STD_LAST; ++idx)
   {
      if (S_STD_SIGNALS_PROPS[idx].rawSignal == signal)
      {
         *out = idx;
         return true;
      }
   }

   // RT Signals
   if (!(signal < SIGRTMIN || signal > SIGRTMAX))
   {
      *out = PSignal_ENUM_RT_FIRST + (signal - SIGRTMIN);
      return true;
   }

   return false;
}
