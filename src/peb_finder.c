#include "peb_finder.h"

HANDLE obtainProcessHandle(DWORD pid){
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (!hProcess) {
        return NULL;
    }

    return hProcess;
}

HMODULE loadNTDLLModule(){
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    return hNtdll ? hNtdll : LoadLibraryW(L"ntdll.dll");
}

NtQueryInformationProcess_t obtainNtQueryInformationProcessAddress(HMODULE ntdll){
    NtQueryInformationProcess_t NtQueryInformationProcess = (NtQueryInformationProcess_t)GetProcAddress(ntdll, "NtQueryInformationProcess");
    return NtQueryInformationProcess;
}

PEB obtainProcessEnvironmentBlock(NtQueryInformationProcess_t NtQueryInformationProcess, HANDLE hProcess){
    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength = 0;
    PEB peb; 
    SIZE_T bytesRead = 0;

    NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);
    
    ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb), &bytesRead);
    return peb;
}


