#ifndef __PLIST_H__
#define __PLIST_H__

#include "utils.h"
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <processthreadsapi.h>
#include <time.h>
#include <winternl.h> // Needed for NTSTATUS

// Define the NTSTATUS data type if not already defined
#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

// Define the THREAD_BASIC_INFORMATION structure
typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS   ExitStatus;
    PVOID      TebBaseAddress;
    CLIENT_ID  ClientId;
    KAFFINITY  AffinityMask;
    KPRIORITY  Priority;
    KPRIORITY  BasePriority;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

// Define the NtQueryInformationThread function
typedef NTSTATUS(WINAPI* NtQueryInformationThreadFunc)(
    HANDLE ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
);

// Define the NtQueryInformationProcess fucntion
typedef NTSTATUS(WINAPI* NtQueryInformationProcessFunc)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

// Contantes
extern NtQueryInformationThreadFunc pNtQueryInformationThread;
extern NtQueryInformationProcessFunc pNtQueryInformationProcess;

// Forward declarations:
BOOL GetProcessList();

BOOL DisplayProcess(PROCESSENTRY32 pe);

BOOL DisplayThread(THREADENTRY32 te);

BOOL GetProcessByName(char* pName);

BOOL GetProcessDetails(int pid);

ULONGLONG GetProcessCPUTime(DWORD dwPID);

#endif