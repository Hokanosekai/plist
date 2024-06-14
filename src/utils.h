#include <windows.h>
#include <string.h>

// Utils methods définitions
char* FormatBytesToMB(size_t bytes);

const char* FormatTimeStamp(const ULONGLONG timestamp);

ULONGLONG GetElapsedTime(FILETIME ftCreation) ;

const char* GetThreadStateString(ULONG threadState);