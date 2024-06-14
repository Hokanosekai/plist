#include "plist.h"

// Constants
NtQueryInformationThreadFunc pNtQueryInformationThread;
NtQueryInformationProcessFunc pNtQueryInformationProcess;

BOOL DisplayProcess(PROCESSENTRY32 pe) {
    HANDLE hProcess;

    // On récupère l'handle d'un processus via son ID
    hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
    if (hProcess == NULL) {
        printf("[ERROR] Unable to open an handle on process %d\n", pe.th32ProcessID);
        return(FALSE);
    }

    // On récupère les informations sur la mémoire utilisée par le processus
    size_t privMemory = 0;
    PROCESS_MEMORY_COUNTERS pmc;
    pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
    if (K32GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
        privMemory = pmc.WorkingSetSize;
    char* formattedPrivMemory = FormatBytesToMB(privMemory);

    // On récupère le nombre d'handles du processus
    DWORD dwHandleCount;
    GetProcessHandleCount(hProcess, &dwHandleCount);

    // On récupère les informations du temps du processus
    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser);

    // On calcule le temps passé en millisecondes depuis le lancement du processus
    ULONGLONG elapsedTime = GetElapsedTime(ftCreation);

    // On calcul le CPU time du processus
    ULONGLONG CPUTime = GetProcessCPUTime(pe.th32ProcessID);

    // On affiche les différentes valeurs récupérées sur le processus
    _tprintf(TEXT("%-40s "), pe.szExeFile);
    _tprintf(TEXT("%-6d "), pe.th32ProcessID);
    _tprintf(TEXT("%-6d "), pe.pcPriClassBase);
    _tprintf(TEXT("%-6d "), pe.cntThreads);
    _tprintf(TEXT("%-6d "), dwHandleCount);
    printf("%-18s ", formattedPrivMemory);

    printf("%-30s ", FormatTimeStamp(CPUTime));
    printf("%-30s ", FormatTimeStamp(elapsedTime));

    // On ferme l'handle du processus
    CloseHandle(hProcess);
    return(TRUE);
}

BOOL DisplayThread(THREADENTRY32 te) {
    HANDLE hThread;

    // On ouvre une handle sur le thread
    hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te.th32ThreadID);
    // Si il y a un problème lors de l'ouverture de l'handle on return
    if (hThread == NULL) {
        printf("[ERROR] Unable to open an handle on thread %d\n", te.th32ThreadID);
        return(FALSE);
    }

    // Query the thread state
    SYSTEM_THREAD_INFORMATION threadInfo;
    ULONG threadState;
    NTSTATUS status = pNtQueryInformationThread(hThread, (THREADINFOCLASS)0x28, &threadInfo, sizeof(threadInfo), NULL);
    if (status != 0)
        threadState = -1;
    else
        threadState = threadInfo.ThreadState;

    // Get the thread information
    PVOID pThreadStartAddress = NULL;
    status = pNtQueryInformationThread(hThread, (THREADINFOCLASS)0x9, &pThreadStartAddress, sizeof(pThreadStartAddress), NULL);
    if (status != 0)
        return(FALSE);

    // On récupère les informations du temps du thread
    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser);

    // On calcule le temps passé en millisecondes
    ULONGLONG elapsedTime = ((ULONGLONG)ftKernel.dwHighDateTime << 32) | ftKernel.dwLowDateTime +
        ((ULONGLONG)ftUser.dwHighDateTime << 32) | ftUser.dwLowDateTime;


    // On affiche les différentes valeurs que l'on a récupérées sur le thread
    _tprintf(TEXT("%-6d "), te.th32ThreadID);
    _tprintf(TEXT("%-6d "), te.tpBasePri);
    printf("%-10s ", "???");
    printf("%-25s ", GetThreadStateString(threadState));

    printf("0x%-20p ", pThreadStartAddress);
    printf("%-30s ", FormatTimeStamp(elapsedTime));

    // On ferme l'handle du processus
    CloseHandle(hThread);
    return(TRUE);
}

BOOL GetProcessList() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Unable to take process snapshot\n");
        return(FALSE);
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        printf("[ERROR] Failed to get process from snapshot\n");
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return(FALSE);
    }

    // On affiche les colonnes de notre tableau
    printf("%-40s %-6s %-6s %-6s %-6s %-18s %-30s %-30s\n", "Name", "Pid", "Pri", "Thd", "Hnd", "Priv", "CPU Time", "Elapsed Time");

    // On parcours la snapshot des processus
    do {
        if (pe32.th32ProcessID == 0)
            continue;

        // On affiche les informations du processus
        DisplayProcess(pe32);

        // On saute une ligne
        printf("\n");

        // On passe au prochain processus
    } while (Process32Next(hProcessSnap, &pe32));

    // On n'oublie pas de fermer l'handle de la snapshot des processus
    CloseHandle(hProcessSnap);
    return(TRUE);
}

ULONGLONG GetProcessCPUTime(DWORD dwPID) {

    ULONGLONG totalTime = 0;

    HANDLE hThread;
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    THREADENTRY32 te32;

    // On prend une snapshot des threads
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return(FALSE);

    // Fill in the size of the structure before using it. 
    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(hThreadSnap, &te32)) {
        CloseHandle(hThreadSnap);          // clean the snapshot object
        return(FALSE);
    }

    // Now walk the thread list of the system,
    // and display information about each thread
    // associated with the specified process
    do {
        // On check si le thread est un enfant du processus
        if (te32.th32OwnerProcessID != dwPID)
            continue;

        // On ouvre une handle sur le thread
        hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
        if (hThread == NULL)
            continue;

        // On r<écupère les informations de temps du thread
        FILETIME ftCreation, ftExit, ftKernel, ftUser;
        GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser);

        // On calcule le temps passé en millisecondes
        ULONGLONG elapsedTime = ((ULONGLONG)ftKernel.dwHighDateTime << 32) | ftKernel.dwLowDateTime +
            ((ULONGLONG)ftUser.dwHighDateTime << 32) | ftUser.dwLowDateTime;

        // On additionne le temps passé 
        totalTime += elapsedTime;

        // On n'oublie pas de fermée l'handle sur le thread
        CloseHandle(hThread);

        // On passe au prochain thread
    } while (Thread32Next(hThreadSnap, &te32));

    // On ferme l'handle sur la snapshot
    CloseHandle(hThreadSnap);
    return(totalTime);
}

BOOL GetProcessByName(char* pName) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        //printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
        return(FALSE);
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return(FALSE);
    }

    BOOL foundProcess = FALSE;

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    printf("%-40s %-6s %-6s %-6s %-6s %-18s %-30s %-30s\n", "Name", "Pid", "Pri", "Thd", "Hnd", "Priv", "CPU Time", "Elapsed Time");
    do {
        char* tmpName = pName;
        int found = 0;
        size_t nameLen = strnlen_s(tmpName, 100);

        // On cherche si un nom de processus correspond on nom rechercher
        for (size_t i = 0; i < nameLen; i++) {
            if (tolower(tmpName[i]) == tolower(pe32.szExeFile[i])) {
                found++;
                continue;
            }
            break;
        }

        // Si l'on a pas trouvé de processus correspondant à la recherche on passe au processus suivant
        if (found != nameLen)
            continue;

        // On affiche les informations du processus
        DisplayProcess(pe32);

        // On saute une ligne
        _tprintf(TEXT("\n"));

        // On passe au prochain processus
    } while (Process32Next(hProcessSnap, &pe32));

    // On ferme l'handle de la snapshot
    CloseHandle(hProcessSnap);
    return(TRUE);
}

BOOL GetProcessDetails(int pid) {
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
    THREADENTRY32 te32;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all running process
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Unable to take process snapshot\n");
        return(FALSE);
    }

    // Fill kin the size of the structure before using it
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        printf("[ERROR] Failed to get process from snapshot\n");
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return(FALSE);
    }

    do {
        // Si le process n'est pas celui qu'on cherche on passe au suivant
        if (pe32.th32ProcessID != pid)
            continue;

        // On saute une ligne
        _tprintf(TEXT("%s (PID: %d)\n\n"), pe32.szExeFile, pid);

        // On passe au prochain process
    } while (Process32Next(hProcessSnap, &pe32));

    // On ferme l'handle de la snapshot
    CloseHandle(hProcessSnap);


    // Take a snapshot of all running threads  
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Unable to take thread snapshot\n");
        return(FALSE);
    }

    // Fill in the size of the structure before using it. 
    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(hThreadSnap, &te32)) {
        printf("[ERROR] Failed to get thread from snapshot\n");
        CloseHandle(hThreadSnap);          // clean the snapshot object
        return(FALSE);
    }

    // Now walk the thread list of the system,
    // and display information about each thread
    // associated with the specified process
    printf("%-6s %-6s %-10s %-25s %-20s %-30s\n", "Tid", "Pri", "Cswtch", "State", "Entry Point", "Elapsed Time");
    do {
        // Si le thread n'est pas l'enfant du pid recherché on passe au suivant
        if (te32.th32OwnerProcessID != pid)
            continue;

        // On affiche les informations du thread
        DisplayThread(te32);

        // On saute une ligne
        _tprintf(TEXT("\n"));

        // On passe au prochain thread
    } while (Thread32Next(hThreadSnap, &te32));

    // On ferme l'handle de la snapshot
    CloseHandle(hThreadSnap);
    return(TRUE);
}