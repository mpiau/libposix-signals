#define _GNU_SOURCE

#include "libposix_signals/posix_signals.h"
#include "libposix_signals/posix_signal_bitmasks.h"
#include "libmacros/macro_utils.h"

#include <signal.h>
#include <stddef.h>
#include <unistd.h>


//================================================================================================
// Internal Data
//================================================================================================

struct StdSigLookupTable
{
   PSignal psig;
   int     rawSig;
};
typedef struct StdSigLookupTable StdSigLookupTable;

static constexpr StdSigLookupTable S_STDSIG_LOOKUP_TABLE[] = 
{
     (StdSigLookupTable){ PSignal_SIGHUP,    SIGHUP    }
   , (StdSigLookupTable){ PSignal_SIGINT,    SIGINT    }
   , (StdSigLookupTable){ PSignal_SIGQUIT,   SIGQUIT   }
   , (StdSigLookupTable){ PSignal_SIGILL,    SIGILL    }
   , (StdSigLookupTable){ PSignal_SIGTRAP,   SIGTRAP   }
   , (StdSigLookupTable){ PSignal_SIGABRT,   SIGABRT   }
   , (StdSigLookupTable){ PSignal_SIGBUS,    SIGBUS    }
   , (StdSigLookupTable){ PSignal_SIGFPE,    SIGFPE    }
   , (StdSigLookupTable){ PSignal_SIGKILL,   SIGKILL   }
   , (StdSigLookupTable){ PSignal_SIGUSR1,   SIGUSR1   }
   , (StdSigLookupTable){ PSignal_SIGSEGV,   SIGSEGV   }
   , (StdSigLookupTable){ PSignal_SIGUSR2,   SIGUSR2   }
   , (StdSigLookupTable){ PSignal_SIGPIPE,   SIGPIPE   }
   , (StdSigLookupTable){ PSignal_SIGALRM,   SIGALRM   }
   , (StdSigLookupTable){ PSignal_SIGTERM,   SIGTERM   }
   , (StdSigLookupTable){ PSignal_SIGSTKFLT, SIGSTKFLT }
   , (StdSigLookupTable){ PSignal_SIGCHLD,   SIGCHLD   }
   , (StdSigLookupTable){ PSignal_SIGCONT,   SIGCONT   }
   , (StdSigLookupTable){ PSignal_SIGSTOP,   SIGSTOP   }
   , (StdSigLookupTable){ PSignal_SIGTSTP,   SIGTSTP   }
   , (StdSigLookupTable){ PSignal_SIGTTIN,   SIGTTIN   }
   , (StdSigLookupTable){ PSignal_SIGTTOU,   SIGTTOU   }
   , (StdSigLookupTable){ PSignal_SIGURG,    SIGURG    }
   , (StdSigLookupTable){ PSignal_SIGXCPU,   SIGXCPU   }
   , (StdSigLookupTable){ PSignal_SIGXFSZ,   SIGXFSZ   }
   , (StdSigLookupTable){ PSignal_SIGVTALRM, SIGVTALRM }
   , (StdSigLookupTable){ PSignal_SIGPROF,   SIGPROF   }
   , (StdSigLookupTable){ PSignal_SIGWINCH,  SIGWINCH  }
   , (StdSigLookupTable){ PSignal_SIGIO,     SIGIO     }
   , (StdSigLookupTable){ PSignal_SIGPWR,    SIGPWR    }
   , (StdSigLookupTable){ PSignal_SIGSYS,    SIGSYS    }
};

struct SigSymbols
{
   char const *name;
   char const *desc;
};
typedef struct SigSymbols SigSymbols;

static constexpr SigSymbols S_SIGNAL_SYMBOLS[] =
{
     (SigSymbols){ "SIGHUP",    "Terminal Hang-Up / Process Death Detected"     }
   , (SigSymbols){ "SIGINT",    "User Interrupt (Ctrl+C)"                       }
   , (SigSymbols){ "SIGQUIT",   "Quit from keyboard"                            }
   , (SigSymbols){ "SIGILL",    "Illegal Instruction"                           }
   , (SigSymbols){ "SIGTRAP",   "Trace / Breakpoint trap"                       }
   , (SigSymbols){ "SIGABRT",   "Abort signal"                                  }
   , (SigSymbols){ "SIGBUS",    "Bus error (bad memory access)"                 }
   , (SigSymbols){ "SIGFPE",    "Erroneous arithmetic operation"                }
   , (SigSymbols){ "SIGKILL",   "Kill signal"                                   }
   , (SigSymbols){ "SIGUSR1",   "User-defined signal 1"                         }
   , (SigSymbols){ "SIGSEGV",   "Invalid memory reference (Segmentation Fault)" }
   , (SigSymbols){ "SIGUSR2",   "User-defined signal 2"                         }
   , (SigSymbols){ "SIGPIPE",   "Broken pipe: write to pipe with no readers"    }
   , (SigSymbols){ "SIGALRM",   "Timer signal"                                  }
   , (SigSymbols){ "SIGTERM",   "Termination signal"                            }
   , (SigSymbols){ "SIGSTKFLT", "Stack fault on coprocessor"                    }
   , (SigSymbols){ "SIGCHLD",   "Child stopped, terminated, or continued"       }
   , (SigSymbols){ "SIGCONT",   "Continue if stopped"                           }
   , (SigSymbols){ "SIGSTOP",   "Stop process"                                  }
   , (SigSymbols){ "SIGTSTP",   "Stop typed at terminal"                        }
   , (SigSymbols){ "SIGTTIN",   "Terminal input for background process"         }
   , (SigSymbols){ "SIGTTOU",   "Terminal output for background process"        }
   , (SigSymbols){ "SIGURG",    "Urgent condition on socket"                    }
   , (SigSymbols){ "SIGXCPU",   "CPU time limit exceeded"                       }
   , (SigSymbols){ "SIGXFSZ",   "File size limit exceeded"                      }
   , (SigSymbols){ "SIGVTALRM", "Virtual alarm clock"                           }
   , (SigSymbols){ "SIGPROF",   "Profiling timer expired"                       }
   , (SigSymbols){ "SIGWINCH",  "Window resize signal"                          }
   , (SigSymbols){ "SIGIO",     "I/O now possible"                              }
   , (SigSymbols){ "SIGPWR",    "Power failure (System V)"                      }
   , (SigSymbols){ "SIGSYS",    "Bad system call (SVr4)"                        }

   , (SigSymbols){ "SIGRTMIN",      "Real-time signal 0"  }
   , (SigSymbols){ "SIGRTMIN + 1",  "Real-time signal 1"  }
   , (SigSymbols){ "SIGRTMIN + 2",  "Real-time signal 2"  }
   , (SigSymbols){ "SIGRTMIN + 3",  "Real-time signal 3"  }
   , (SigSymbols){ "SIGRTMIN + 4",  "Real-time signal 4"  }
   , (SigSymbols){ "SIGRTMIN + 5",  "Real-time signal 5"  }
   , (SigSymbols){ "SIGRTMIN + 6",  "Real-time signal 6"  }
   , (SigSymbols){ "SIGRTMIN + 7",  "Real-time signal 7"  }
   , (SigSymbols){ "SIGRTMIN + 8",  "Real-time signal 8"  }
   , (SigSymbols){ "SIGRTMIN + 9",  "Real-time signal 9"  }
   , (SigSymbols){ "SIGRTMIN + 10", "Real-time signal 10" }
   , (SigSymbols){ "SIGRTMIN + 11", "Real-time signal 11" }
   , (SigSymbols){ "SIGRTMIN + 12", "Real-time signal 12" }
   , (SigSymbols){ "SIGRTMIN + 13", "Real-time signal 13" }
   , (SigSymbols){ "SIGRTMIN + 14", "Real-time signal 14" }
   , (SigSymbols){ "SIGRTMIN + 15", "Real-time signal 15" }

   , (SigSymbols){ "SIGRTMAX - 14", "Real-time signal 16" }
   , (SigSymbols){ "SIGRTMAX - 13", "Real-time signal 17" }
   , (SigSymbols){ "SIGRTMAX - 12", "Real-time signal 18" }
   , (SigSymbols){ "SIGRTMAX - 11", "Real-time signal 19" }
   , (SigSymbols){ "SIGRTMAX - 10", "Real-time signal 20" }
   , (SigSymbols){ "SIGRTMAX - 9",  "Real-time signal 21" }
   , (SigSymbols){ "SIGRTMAX - 8",  "Real-time signal 22" }
   , (SigSymbols){ "SIGRTMAX - 7",  "Real-time signal 23" }
   , (SigSymbols){ "SIGRTMAX - 6",  "Real-time signal 24" }
   , (SigSymbols){ "SIGRTMAX - 5",  "Real-time signal 25" }
   , (SigSymbols){ "SIGRTMAX - 4",  "Real-time signal 26" }
   , (SigSymbols){ "SIGRTMAX - 3",  "Real-time signal 27" }
   , (SigSymbols){ "SIGRTMAX - 2",  "Real-time signal 28" }
   , (SigSymbols){ "SIGRTMAX - 1",  "Real-time signal 29" }
   , (SigSymbols){ "SIGRTMAX",      "Real-time signal 30" }
};

static_assert(array_capacity(S_SIGNAL_SYMBOLS) == PSignal_Count);


//================================================================================================
// Public API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Identification
//------------------------------------------------------------------------------------------------

bool psignal_validate(unsigned const v)
{
   static_assert(type_is_unsigned(PSignal), "This function assumes PSignal being unsigned.");
   return v < PSignal_Count;
}


//------------------------------------------------------------------------------------------------
// Usage
//------------------------------------------------------------------------------------------------

bool psignal_raise(PSignal const psig)
{
   return psignal_raise_on_pid(psig, getpid());
}

bool psignal_raise_on_pid(PSignal const psig, pid_t const pid)
{
   int const sig = psignal_into_raw_signal(psig);
   if (psignal_is_standard(psig))
   {
      return kill(pid, sig) == 0;
   }
   else
   {
      return sigqueue(pid, sig, (union sigval){}) == 0;
   }
}


//------------------------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------------------------

bool psignal_is_standard(PSignal const psig)
{
   return PSIG_BITMASK_STANDARD_SIGNALS & (1ul << psig);
}

bool psignal_is_real_time(PSignal const psig)
{
   return PSIG_BITMASK_REALTIME_SIGNALS & (1ul << psig);
}

int psignal_into_raw_signal(PSignal const psig)
{
   return psignal_is_standard(psig)
      ? S_STDSIG_LOOKUP_TABLE[psig].rawSig
      : SIGRTMIN + (psig - PSignal_SIGRTMIN);
}

char const *psignal_name(PSignal const psig)
{
   return S_SIGNAL_SYMBOLS[psig].name;
}

char const *psignal_desc(PSignal const psig)
{
   return S_SIGNAL_SYMBOLS[psig].desc;
}


//------------------------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------------------------

bool psignal_from_raw_signal(int const sig, PSignal *const out)
{
   if (!(sig < SIGRTMIN || sig > SIGRTMAX))
   {
      *out = PSignal_SIGRTMIN + (sig - SIGRTMIN);
      return true;
   }

   constexpr unsigned TABLE_SIZE = array_capacity(S_STDSIG_LOOKUP_TABLE);
   for (unsigned idx = 0; idx < TABLE_SIZE; ++idx)
   {
      StdSigLookupTable const *elem = &S_STDSIG_LOOKUP_TABLE[idx];
      if (elem->rawSig == sig)
      {
         *out = elem->psig;
         return true;
      }
   }

   return false;
}
