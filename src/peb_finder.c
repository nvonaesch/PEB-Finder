#include "peb_finder.h"

HANDLE obtainProcessHandle(DWORD pid){
    static char buffer[128];
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (!hProcess) {
        DWORD error = GetLastError();
        sprintf(buffer, "impossible d'ouvrir le PID %lu (code %lu)", pid, error);
        return NULL;
    }

    return hProcess;
}