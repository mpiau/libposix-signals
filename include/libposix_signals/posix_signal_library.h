#pragma once

//================================================================================================
// POSIX Signal Library
//================================================================================================


[[nodiscard]] bool psig_library_init(void);
[[nodiscard]] bool psig_library_is_running(void);
void psig_library_shutdown(void);

[[nodiscard]] char const *psig_library_description(void);

