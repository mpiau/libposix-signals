#pragma once

//================================================================================================
// POSIX Signal Dispositions
//================================================================================================

enum PSigDisp : unsigned char
{
     PSigDisp_CONTINUE  // Continue the process if it's currently stopped.
   , PSigDisp_CORE_DUMP // Terminate the process and produces core dump file.
   , PSigDisp_IGNORE    // Ignore the signal.
   , PSigDisp_STOP      // Stop the process.
   , PSigDisp_TERMINATE // Terminate the process.
};

typedef enum PSigDisp PSigDisp;
