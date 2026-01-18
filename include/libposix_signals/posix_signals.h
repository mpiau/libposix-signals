#pragma once

//================================================================================================
// POSIX Signals
//================================================================================================

/*
   A POSIX signal is an asynchronous mechanism defined by the POSIX standard and used by the OS to
   inform a process that an event has occurred.
   Events such as Invalid memory access, arithmetic errors, termination requests, ...

   There are 2 types of POSIX signals: Standards and Real-Time user defined.
   - Standard signals have fixed semantics, limited queuing, and predefined default dispositions.
   - RealTime signals are queued reliably, delivered in a defined order, and reserved for
   application‑specific usage. Their behaviour is fully modifiable unlike standard ones.

   All these signals are defined as macros (with some determined only at runtime), their numeric
   values are non‑contiguous and vary across operating systems.
   To avoid polluting the global namespace with these macros and to abstract away the platform
   specific complexity, PSignal can be used as a reliable, uniform representation instead.
*/


typedef enum PSignal : unsigned char
{
   //---------------------------------------------------------------------------------------------
   // POSIX Standard Signals
   //---------------------------------------------------------------------------------------------

     PSignal_SIGHUP    // Terminal Hang-Up / Process Death Detected
   , PSignal_SIGINT    // User Interrupt (Ctrl+C)
   , PSignal_SIGQUIT   // Quit from keyboard
   , PSignal_SIGILL    // Illegal Instruction
   , PSignal_SIGTRAP   // Trace / Breakpoint trap
   , PSignal_SIGABRT   // Abort signal
   , PSignal_SIGBUS    // Bus error (bad memory access)
   , PSignal_SIGFPE    // Erroneous arithmetic operation
   , PSignal_SIGKILL   // Kill signal
   , PSignal_SIGUSR1   // User-defined signal 1
   , PSignal_SIGSEGV   // Invalid memory reference (Segmentation Fault)
   , PSignal_SIGUSR2   // User-defined signal 2
   , PSignal_SIGPIPE   // Broken pipe: write to pipe with no readers
   , PSignal_SIGALRM   // Timer signal
   , PSignal_SIGTERM   // Termination signal
   , PSignal_SIGSTKFLT // Stack fault on coprocessor
   , PSignal_SIGCHLD   // Child stopped, terminated, or continued
   , PSignal_SIGCONT   // Continue if stopped
   , PSignal_SIGSTOP   // Stop process
   , PSignal_SIGTSTP   // Stop typed at terminal
   , PSignal_SIGTTIN   // Terminal input for background process
   , PSignal_SIGTTOU   // Terminal output for background process
   , PSignal_SIGURG    // Urgent condition on socket
   , PSignal_SIGXCPU   // CPU time limit exceeded
   , PSignal_SIGXFSZ   // File size limit exceeded
   , PSignal_SIGVTALRM // Virtual alarm clock
   , PSignal_SIGPROF   // Profiling timer expired
   , PSignal_SIGWINCH  // Window resize signal
   , PSignal_SIGIO     // I/O now possible
   , PSignal_SIGPWR    // Power failure (System V)
   , PSignal_SIGSYS    // Bad system call (SVr4)

   // Common aliases

   , PSignal_SIGIOT    = PSignal_SIGABRT
   , PSignal_SIGCLD    = PSignal_SIGCHLD
   , PSignal_SIGPOLL   = PSignal_SIGIO
   , PSignal_SIGUNUSED = PSignal_SIGSYS


   //---------------------------------------------------------------------------------------------
   // POSIX Real-time (user-defined) Signals
   //---------------------------------------------------------------------------------------------

   , PSignal_SIGRTMIN    // Real-time signal 0
   , PSignal_SIGRTMIN_1  // Real-time signal 1
   , PSignal_SIGRTMIN_2  // Real-time signal 2
   , PSignal_SIGRTMIN_3  // Real-time signal 3
   , PSignal_SIGRTMIN_4  // Real-time signal 4
   , PSignal_SIGRTMIN_5  // Real-time signal 5
   , PSignal_SIGRTMIN_6  // Real-time signal 6
   , PSignal_SIGRTMIN_7  // Real-time signal 7
   , PSignal_SIGRTMIN_8  // Real-time signal 8
   , PSignal_SIGRTMIN_9  // Real-time signal 9
   , PSignal_SIGRTMIN_10 // Real-time signal 10
   , PSignal_SIGRTMIN_11 // Real-time signal 11
   , PSignal_SIGRTMIN_12 // Real-time signal 12
   , PSignal_SIGRTMIN_13 // Real-time signal 13
   , PSignal_SIGRTMIN_14 // Real-time signal 14
   , PSignal_SIGRTMIN_15 // Real-time signal 15

   , PSignal_SIGRTMAX_14 // Real-time signal 16
   , PSignal_SIGRTMAX_13 // Real-time signal 17
   , PSignal_SIGRTMAX_12 // Real-time signal 18
   , PSignal_SIGRTMAX_11 // Real-time signal 19
   , PSignal_SIGRTMAX_10 // Real-time signal 20
   , PSignal_SIGRTMAX_9  // Real-time signal 21
   , PSignal_SIGRTMAX_8  // Real-time signal 22
   , PSignal_SIGRTMAX_7  // Real-time signal 23
   , PSignal_SIGRTMAX_6  // Real-time signal 24
   , PSignal_SIGRTMAX_5  // Real-time signal 25
   , PSignal_SIGRTMAX_4  // Real-time signal 26
   , PSignal_SIGRTMAX_3  // Real-time signal 27
   , PSignal_SIGRTMAX_2  // Real-time signal 28
   , PSignal_SIGRTMAX_1  // Real-time signal 29
   , PSignal_SIGRTMAX    // Real-time signal 30
} PSignal;

static constexpr unsigned PSignal_ENUM_FIRST     = PSignal_SIGHUP;
static constexpr unsigned PSignal_ENUM_LAST      = PSignal_SIGRTMAX;
static constexpr unsigned PSignal_ENUM_COUNT     = (PSignal_ENUM_LAST - PSignal_ENUM_FIRST) + 1;

static constexpr unsigned PSignal_ENUM_STD_FIRST = PSignal_SIGHUP;
static constexpr unsigned PSignal_ENUM_STD_LAST  = PSignal_SIGSYS;
static constexpr unsigned PSignal_ENUM_STD_COUNT = (PSignal_ENUM_STD_LAST - PSignal_ENUM_STD_FIRST) + 1;

static constexpr unsigned PSignal_ENUM_RT_FIRST  = PSignal_SIGRTMIN;
static constexpr unsigned PSignal_ENUM_RT_LAST   = PSignal_SIGRTMAX;
static constexpr unsigned PSignal_ENUM_RT_COUNT  = (PSignal_ENUM_RT_LAST - PSignal_ENUM_RT_FIRST) + 1;


//================================================================================================
// API Functions
//================================================================================================

//------------------------------------------------------------------------------------------------
// Identification
//------------------------------------------------------------------------------------------------

/*
   Validate that the given value is correctly defined in the enum.
   IMPORTANT: 
   - Always validate first that your value is valid before converting it to PSignal.
   - Enum values != raw signal value. Use psignal_from_raw_signal instead for raw signal values.
   - All the functions taking a PSignal WILL ASSUME that the given enum value is VALID.
*/
[[nodiscard]]
bool psignal_validate(unsigned);


//------------------------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------------------------

/*
   Returns true if the given PSignal is mapped to a standard POSIX signal.
*/
[[nodiscard]]
bool psignal_is_standard(PSignal);

/*
   Returns true if the given PSignal is mapped to a Real-Time POSIX signal.
*/
[[nodiscard]]
bool psignal_is_real_time(PSignal);

/*
   Returns the "raw" signal value mapped to the enum.
   Example: PSignal_SIGINT will returns the value defined by SIGINT macro.
*/
[[nodiscard]]
int psignal_to_raw_signal(PSignal);

/*
   Returns the name associated to a given PSignal.
   Example: PSignal_SIGSEGV will returns "SIGSEGV".
*/
[[nodiscard]]
char const *psignal_name(PSignal);

/*
   Returns the description associated to a given PSignal.
   Example: PSignal_SIGSEGV will returns "Invalid memory reference (Segmentation Fault)"
*/
[[nodiscard]]
char const *psignal_desc(PSignal);


//------------------------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------------------------

/*
   Try to convert the given raw signal value into one of the defined PSignal value.
   If the raw signal is unknown, false will be returned and nothing will be set in pointed param.
   Example: Given SIGINT, PSignal_SIGINT will be set and true returned.
   Example: Given 0xFFFF, false will be returned.
*/
[[nodiscard]]
bool psignal_from_raw_signal(int, PSignal *);
