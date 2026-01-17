#include "libposix_signals/posix_signals.h"
#include "libposix_signals/posix_signal_dispositions.h"

#include <assert.h>
#include <signal.h>


//================================================================================================
// Internal Data
//================================================================================================

struct StdSigProperties
{
   int rawSignal;
   PSigDisp disposition;
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
     CG_STDSIG_PROPS(PSignal_SIGHUP,    SIGHUP,    PSigDisp_TERMINATE, "Terminal Hang-Up / Process Death Detected")
   , CG_STDSIG_PROPS(PSignal_SIGINT,    SIGINT,    PSigDisp_TERMINATE, "User Interrupt (Ctrl+C)")
   , CG_STDSIG_PROPS(PSignal_SIGQUIT,   SIGQUIT,   PSigDisp_CORE_DUMP, "Quit from keyboard")
   , CG_STDSIG_PROPS(PSignal_SIGILL,    SIGILL,    PSigDisp_CORE_DUMP, "Illegal Instruction")
   , CG_STDSIG_PROPS(PSignal_SIGTRAP,   SIGTRAP,   PSigDisp_CORE_DUMP, "Trace / Breakpoint trap")
   , CG_STDSIG_PROPS(PSignal_SIGABRT,   SIGABRT,   PSigDisp_CORE_DUMP, "Abort signal")
   , CG_STDSIG_PROPS(PSignal_SIGBUS,    SIGBUS,    PSigDisp_CORE_DUMP, "Bus error (bad memory access)")
   , CG_STDSIG_PROPS(PSignal_SIGFPE,    SIGFPE,    PSigDisp_CORE_DUMP, "Erroneous arithmetic operation")
   , CG_STDSIG_PROPS(PSignal_SIGKILL,   SIGKILL,   PSigDisp_TERMINATE, "Kill signal")
   , CG_STDSIG_PROPS(PSignal_SIGUSR1,   SIGUSR1,   PSigDisp_TERMINATE, "User-defined signal 1")
   , CG_STDSIG_PROPS(PSignal_SIGSEGV,   SIGSEGV,   PSigDisp_CORE_DUMP, "Invalid memory reference (Segmentation Fault)")
   , CG_STDSIG_PROPS(PSignal_SIGUSR2,   SIGUSR2,   PSigDisp_TERMINATE, "User-defined signal 2")
   , CG_STDSIG_PROPS(PSignal_SIGPIPE,   SIGPIPE,   PSigDisp_TERMINATE, "Broken pipe: write to pipe with no readers")
   , CG_STDSIG_PROPS(PSignal_SIGALRM,   SIGALRM,   PSigDisp_TERMINATE, "Timer signal")
   , CG_STDSIG_PROPS(PSignal_SIGTERM,   SIGTERM,   PSigDisp_TERMINATE, "Termination signal")
   , CG_STDSIG_PROPS(PSignal_SIGSTKFLT, SIGSTKFLT, PSigDisp_TERMINATE, "Stack fault on coprocessor")
   , CG_STDSIG_PROPS(PSignal_SIGCHLD,   SIGCHLD,   PSigDisp_CONTINUE,  "Child stopped, terminated, or continued")
   , CG_STDSIG_PROPS(PSignal_SIGCONT,   SIGCONT,   PSigDisp_CONTINUE,  "Continue if stopped")
   , CG_STDSIG_PROPS(PSignal_SIGSTOP,   SIGSTOP,   PSigDisp_STOP,      "Stop process")
   , CG_STDSIG_PROPS(PSignal_SIGTSTP,   SIGTSTP,   PSigDisp_STOP,      "Stop typed at terminal")
   , CG_STDSIG_PROPS(PSignal_SIGTTIN,   SIGTTIN,   PSigDisp_STOP,      "Terminal input for background process")
   , CG_STDSIG_PROPS(PSignal_SIGTTOU,   SIGTTOU,   PSigDisp_STOP,      "Terminal output for background process")
   , CG_STDSIG_PROPS(PSignal_SIGURG,    SIGURG,    PSigDisp_IGNORE,    "Urgent condition on socket")
   , CG_STDSIG_PROPS(PSignal_SIGXCPU,   SIGXCPU,   PSigDisp_CORE_DUMP, "CPU time limit exceeded")
   , CG_STDSIG_PROPS(PSignal_SIGXFSZ,   SIGXFSZ,   PSigDisp_CORE_DUMP, "File size limit exceeded")
   , CG_STDSIG_PROPS(PSignal_SIGVTALRM, SIGVTALRM, PSigDisp_TERMINATE, "Virtual alarm clock")
   , CG_STDSIG_PROPS(PSignal_SIGPROF,   SIGPROF,   PSigDisp_TERMINATE, "Profiling timer expired")
   , CG_STDSIG_PROPS(PSignal_SIGWINCH,  SIGWINCH,  PSigDisp_IGNORE,    "Window resize signal")
   , CG_STDSIG_PROPS(PSignal_SIGIO,     SIGIO,     PSigDisp_TERMINATE, "I/O now possible")
   , CG_STDSIG_PROPS(PSignal_SIGPWR,    SIGPWR,    PSigDisp_TERMINATE, "Power failure (System V)")
   , CG_STDSIG_PROPS(PSignal_SIGSYS,    SIGSYS,    PSigDisp_CORE_DUMP, "Bad system call (SVr4)")
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


static_assert(arrayCapacity(S_STD_SIGNALS_PROPS) == PSignal_EnumStandardCount);
static_assert(arrayCapacity(S_RT_SIGNALS_PROPS)  == PSignal_EnumRealTimeCount);


//================================================================================================
// API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Identification
//------------------------------------------------------------------------------------------------

bool psignal_validate(int const v)
{
   return !(v < PSignal_EnumFirst || v > PSignal_EnumLast);
}


//------------------------------------------------------------------------------------------------
// POSIX Signal Properties
//------------------------------------------------------------------------------------------------

bool psignal_is_real_time(PSignal const psig)
{
   return !(psig < PSignal_EnumRealTimeFirst || psig > PSignal_EnumRealTimeLast);
}

bool psignal_is_standard(PSignal const psig)
{
   return !(psig < PSignal_EnumStandardFirst || psig > PSignal_EnumStandardLast);
}


int psignal_associated_raw_signal(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[psig].rawSignal
      : SIGRTMIN + (psig - PSignal_EnumRealTimeFirst);
}

bool psignal_is_hookable(PSignal const psig)
{
   return (psig != PSignal_SIGKILL && psig != PSignal_SIGSTOP);
}

PSigDisp psignal_default_disposition(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[psig].disposition
      : PSigDisp_IGNORE;
}


char const *psignal_name(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[psig].name
      : S_RT_SIGNALS_PROPS[psig - PSignal_EnumRealTimeFirst].name;
}

char const *psignal_desc(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STD_SIGNALS_PROPS[psig].desc
      : S_RT_SIGNALS_PROPS[psig - PSignal_EnumRealTimeFirst].desc;
}


//------------------------------------------------------------------------------------------------
// Conversion functions
//------------------------------------------------------------------------------------------------

bool psignal_try_from_raw_signal(int const signal, PSignal *const out)
{
   // STD Signals
   for (PSignal idx = PSignal_EnumStandardFirst; idx <= PSignal_EnumStandardLast; ++idx)
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
      *out = PSignal_EnumRealTimeFirst + (signal - SIGRTMIN);
      assert(*out <= PSignal_EnumRealTimeLast);
      return true;
   }

   return false;
}
