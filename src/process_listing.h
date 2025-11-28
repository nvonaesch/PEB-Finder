#pragma once
#include <windows.h>
#include <stdio.h>
#include "TlHelp32.h"

typedef struct _listedProcess {
    wchar_t* processName;
    DWORD pid;
    DWORD parentPid;
} listedProcess, *plistedProcess;

listedProcess* listProcesses(size_t* count);