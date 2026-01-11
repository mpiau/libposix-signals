#pragma once


//================================================================================================
// POSIX Signal Dispositions
//================================================================================================

enum PSignalDisposition : unsigned char
{
     PSignalDisposition_CONTINUE  // Continue the process if it's currently stopped.
   , PSignalDisposition_CORE_DUMP // Terminate the process and produces core dump file.
   , PSignalDisposition_IGNORE    // Ignore the signal.
   , PSignalDisposition_STOP      // Stop the process.
   , PSignalDisposition_TERMINATE // Terminate the process.
};

typedef enum PSignalDisposition PSignalDisposition;
