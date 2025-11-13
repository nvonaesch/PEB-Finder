#include "peb_finder.h"

HANDLE obtainProcessHandle(DWORD pid){
    static char buffer[128];
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

PEB obtainProcessEnvironmentBlock(HMODULE ntdll, NtQueryInformationProcess_t NtQueryInformationProcess){
    

}


