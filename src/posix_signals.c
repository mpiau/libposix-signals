#include "libposix_signals/posix_signals.h"
#include "libposix_signals/posix_signal_dispositions.h"

#include <assert.h>
#include <signal.h>


//================================================================================================
// Private Data
//================================================================================================

struct StdSigProperties
{
   int rawSignal;
   PSigDisposition disposition;
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


#define arrayCapacity(pArray) (sizeof(pArray) / sizeof(pArray[0]))

#define CG_STDSIG_PROPS(pSig, pRaw, pDisp, pDesc)  \
   [pSig] = (StdSigProperties) { \
      .rawSignal = pRaw,         \
      .disposition = pDisp,      \
      .name = #pRaw,             \
      .desc = pDesc              \
   }
static constexpr StdSigProperties S_STD_SIGNALS_PROPS[] =
{
     CG_STDSIG_PROPS(PSignal_SIGHUP,    SIGHUP,    PSigDisposition_TERMINATE, "Terminal Hang-Up / Process Death Detected")
   , CG_STDSIG_PROPS(PSignal_SIGINT,    SIGINT,    PSigDisposition_TERMINATE, "User Interrupt (Ctrl+C)")
   , CG_STDSIG_PROPS(PSignal_SIGQUIT,   SIGQUIT,   PSigDisposition_CORE_DUMP, "Quit from keyboard")
   , CG_STDSIG_PROPS(PSignal_SIGILL,    SIGILL,    PSigDisposition_CORE_DUMP, "Illegal Instruction")
   , CG_STDSIG_PROPS(PSignal_SIGTRAP,   SIGTRAP,   PSigDisposition_CORE_DUMP, "Trace / Breakpoint trap")
   , CG_STDSIG_PROPS(PSignal_SIGABRT,   SIGABRT,   PSigDisposition_CORE_DUMP, "Abort signal")
   , CG_STDSIG_PROPS(PSignal_SIGBUS,    SIGBUS,    PSigDisposition_CORE_DUMP, "Bus error (bad memory access)")
   , CG_STDSIG_PROPS(PSignal_SIGFPE,    SIGFPE,    PSigDisposition_CORE_DUMP, "Erroneous arithmetic operation")
   , CG_STDSIG_PROPS(PSignal_SIGKILL,   SIGKILL,   PSigDisposition_TERMINATE, "Kill signal")
   , CG_STDSIG_PROPS(PSignal_SIGUSR1,   SIGUSR1,   PSigDisposition_TERMINATE, "User-defined signal 1")
   , CG_STDSIG_PROPS(PSignal_SIGSEGV,   SIGSEGV,   PSigDisposition_CORE_DUMP, "Invalid memory reference (Segmentation Fault)")
   , CG_STDSIG_PROPS(PSignal_SIGUSR2,   SIGUSR2,   PSigDisposition_TERMINATE, "User-defined signal 2")
   , CG_STDSIG_PROPS(PSignal_SIGPIPE,   SIGPIPE,   PSigDisposition_TERMINATE, "Broken pipe: write to pipe with no readers")
   , CG_STDSIG_PROPS(PSignal_SIGALRM,   SIGALRM,   PSigDisposition_TERMINATE, "Timer signal")
   , CG_STDSIG_PROPS(PSignal_SIGTERM,   SIGTERM,   PSigDisposition_TERMINATE, "Termination signal")
   , CG_STDSIG_PROPS(PSignal_SIGSTKFLT, SIGSTKFLT, PSigDisposition_TERMINATE, "Stack fault on coprocessor")
   , CG_STDSIG_PROPS(PSignal_SIGCHLD,   SIGCHLD,   PSigDisposition_CONTINUE,  "Child stopped, terminated, or continued")
   , CG_STDSIG_PROPS(PSignal_SIGCONT,   SIGCONT,   PSigDisposition_CONTINUE,  "Continue if stopped")
   , CG_STDSIG_PROPS(PSignal_SIGSTOP,   SIGSTOP,   PSigDisposition_STOP,      "Stop process")
   , CG_STDSIG_PROPS(PSignal_SIGTSTP,   SIGTSTP,   PSigDisposition_STOP,      "Stop typed at terminal")
   , CG_STDSIG_PROPS(PSignal_SIGTTIN,   SIGTTIN,   PSigDisposition_STOP,      "Terminal input for background process")
   , CG_STDSIG_PROPS(PSignal_SIGTTOU,   SIGTTOU,   PSigDisposition_STOP,      "Terminal output for background process")
   , CG_STDSIG_PROPS(PSignal_SIGURG,    SIGURG,    PSigDisposition_IGNORE,    "Urgent condition on socket")
   , CG_STDSIG_PROPS(PSignal_SIGXCPU,   SIGXCPU,   PSigDisposition_CORE_DUMP, "CPU time limit exceeded")
   , CG_STDSIG_PROPS(PSignal_SIGXFSZ,   SIGXFSZ,   PSigDisposition_CORE_DUMP, "File size limit exceeded")
   , CG_STDSIG_PROPS(PSignal_SIGVTALRM, SIGVTALRM, PSigDisposition_TERMINATE, "Virtual alarm clock")
   , CG_STDSIG_PROPS(PSignal_SIGPROF,   SIGPROF,   PSigDisposition_TERMINATE, "Profiling timer expired")
   , CG_STDSIG_PROPS(PSignal_SIGWINCH,  SIGWINCH,  PSigDisposition_IGNORE,    "Window resize signal")
   , CG_STDSIG_PROPS(PSignal_SIGIO,     SIGIO,     PSigDisposition_TERMINATE, "I/O now possible")
   , CG_STDSIG_PROPS(PSignal_SIGPWR,    SIGPWR,    PSigDisposition_TERMINATE, "Power failure (System V)")
   , CG_STDSIG_PROPS(PSignal_SIGSYS,    SIGSYS,    PSigDisposition_CORE_DUMP, "Bad system call (SVr4)")
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


static_assert(arrayCapacity(S_STD_SIGNALS_PROPS) == PSignal_ENUM_STD_COUNT);
static_assert(arrayCapacity(S_RT_SIGNALS_PROPS)  == PSignal_ENUM_RT_COUNT);


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

bool psignal_is_real_time(PSignal const psig)
{
   return !(psig < PSignal_ENUM_RT_FIRST || psig > PSignal_ENUM_RT_LAST);
}

bool psignal_is_standard(PSignal const psig)
{
   // The code below assumes that we start from 0 and enum is unsigned.
   static_assert(PSignal_ENUM_STD_FIRST == 0 && (PSignal)-1 > 0);

   return psig <= PSignal_ENUM_STD_LAST;
}


int psignal_to_raw_signal(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[std_sig_idx(psig)].rawSignal
      : SIGRTMIN + (int)rt_sig_idx(psig);
}


PSigDisposition psignal_default_disposition(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[std_sig_idx(psig)].disposition
      : PSigDisposition_IGNORE;
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
