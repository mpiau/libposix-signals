#pragma once

//================================================================================================
// POSIX Signal Dispositions
//================================================================================================

enum PSigDisposition : unsigned char
{
     PSigDisposition_CONTINUE  // Continue the process if it's currently stopped.
   , PSigDisposition_CORE_DUMP // Terminate the process and produces core dump file.
   , PSigDisposition_IGNORE    // Ignore the signal.
   , PSigDisposition_STOP      // Stop the process.
   , PSigDisposition_TERMINATE // Terminate the process.
};

typedef enum PSigDisposition PSigDisposition;
