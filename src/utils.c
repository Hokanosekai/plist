#include "utils.h"
#include <stdio.h>

// Format bytes to MB
char* FormatBytesToMB(size_t bytes) {
    const int MB = 1024 * 1024;
    static char buf[32];

    // Use double to prevent integer division
    double mb = (double)bytes / MB;

    // Print the MB with up to one decimal place
    snprintf(buf, sizeof(buf), "%.1f MB", mb);

    return buf;
}

// Convert a FILETIME to ULONGLONG
ULONGLONG FileTimeToULL(FILETIME* ft) {
    ULARGE_INTEGER uli;
    uli.LowPart = ft->dwLowDateTime;
    uli.HighPart = ft->dwHighDateTime;
    return uli.QuadPart;
}

ULONGLONG GetElapsedTime(FILETIME ftCreation) {
    // Get the current system time
    FILETIME ftCurrent;
    GetSystemTimeAsFileTime(&ftCurrent);

    // Convert FILETIME to ULONGLONG for easier calculation
    ULONGLONG creationTime = ((ULONGLONG)ftCreation.dwHighDateTime << 32) | ftCreation.dwLowDateTime;
    ULONGLONG currentTime = ((ULONGLONG)ftCurrent.dwHighDateTime << 32) | ftCurrent.dwLowDateTime;

    // Calculate the elapsed time in milliseconds
    ULONGLONG elapsedTime = currentTime - creationTime;

    // Convert elapsed time to milliseconds
    elapsedTime /= 10000; // Convert 100-nanosecond intervals to milliseconds

    return elapsedTime;
}

// Format a timestamp into a readable string
const char* FormatTimeStamp(const ULONGLONG timestamp) {
    // Convert timestamp to milliseconds
    ULONGLONG milliseconds = timestamp;

    // Calculate days, hours, minutes, seconds, and milliseconds
    ULONGLONG seconds = milliseconds / 1000;
    ULONGLONG minutes = seconds / 60;
    ULONGLONG hours = minutes / 60;
    //ULONGLONG days = hours / 24;
    ULONGLONG remainingHours = hours % 24;
    ULONGLONG remainingMinutes = minutes % 60;
    ULONGLONG remainingSeconds = seconds % 60;
    ULONGLONG remainingMilliseconds = milliseconds % 1000;

    // Allocate memory for the formatted string
    char* formattedTimeStamp = (char*)malloc(32 * sizeof(char));

    // Format the timestamp string
    snprintf(formattedTimeStamp, 32, "%02llu:%02llu:%02llu.%03llu", remainingHours, remainingMinutes, remainingSeconds, remainingMilliseconds);

    return formattedTimeStamp;
}

// Get the string representation of the thread state
const char* GetThreadStateString(ULONG threadState) {
    switch (threadState) {
    case -1:
        return "Undefined";
    case 0:
        return "Initialized";
    case 1:
        return "Ready";
    case 2:
        return "Running";
    case 3:
        return "Standby";
    case 4:
        return "Terminated";
    case 5:
        return "Waiting";
    case 6:
        return "Transition";
    case 7:
        return "DeferredReady";
    case 8:
        return "GateWaitObsolete";
    case 9:
        return "WaitingForProcessInSwap";
    case 10:
        return "MaximumThreadState";
    default:
        return "Unknown";
    }
}
