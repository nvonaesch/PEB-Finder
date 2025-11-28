#include "process_listing.h"

listedProcess* listProcesses(size_t* count){
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    Process32FirstW(snapshot, &pe);

    listedProcess* list = NULL;
    size_t n = 0;

    
    do{
        list = realloc(list, sizeof(listedProcess) * (n+1));
        list[n].pid = pe.th32ProcessID;
        list[n].parentPid = pe.th32ParentProcessID;
        list[n].processName = _wcsdup(pe.szExeFile);
        n++;
    }while(Process32NextW(snapshot, &pe));

    CloseHandle(snapshot);
    *count = n;
    return list;
}
