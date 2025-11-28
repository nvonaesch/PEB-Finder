#include "peb_loader.h"
#include "utils.h"

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
    NtQueryInformationProcess_t NtQueryInformationProcess = (NtQueryInformationProcess_t)(void*)GetProcAddress(ntdll, "NtQueryInformationProcess");
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

PEB_LDR_DATA loadLDRData(HANDLE hProcess, PEB peb) {
    PEB_LDR_DATA ldr = {0};
    SIZE_T bytesRead = 0;

    if (!ReadProcessMemory(hProcess, peb.Ldr, &ldr, sizeof(PEB_LDR_DATA), &bytesRead)) {
        wprintf(L"Erreur de lecture du LDR (code %lu)\n", GetLastError());
        return (PEB_LDR_DATA){0};
    }

    return ldr;
}

void showLoadedModules(HWND hListView, HANDLE hProcess, PEB peb) {
    PEB_LDR_DATA ldr = loadLDRData(hProcess, peb);
    LIST_ENTRY* head = ldr.InLoadOrderModuleList.Flink;
    LIST_ENTRY* current = head;

    int index = 0;
    do {
        LDR_DATA_TABLE_ENTRY entry = {0};
        if (!ReadProcessMemory(hProcess, current, &entry, sizeof(entry), NULL))
            break;

        wchar_t baseName[MAX_PATH] = L"";
        if (entry.BaseDllName.Buffer && entry.BaseDllName.Length > 0)
            ReadProcessMemory(hProcess, entry.BaseDllName.Buffer, baseName, entry.BaseDllName.Length, NULL);

        wchar_t fullName[MAX_PATH] = L"";
        if (entry.FullDllName.Buffer && entry.FullDllName.Length > 0)
            ReadProcessMemory(hProcess, entry.FullDllName.Buffer, fullName, entry.FullDllName.Length, NULL);

        wchar_t addrbuf[128];
        swprintf(addrbuf, 128, L"0x%p", entry.DllBase);
        addListViewRow(hListView, baseName[0] ? baseName : L"<inconnu>", addrbuf);

        current = entry.InLoadOrderLinks.Flink;
        index++;
    } while (current != head && index < 256);
}

RTL_USER_PROCESS_PARAMETERS loadTRLData(HANDLE hProcess, PEB peb){
    RTL_USER_PROCESS_PARAMETERS params = {0};
    SIZE_T bytesRead = 0;

    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &params, sizeof(RTL_USER_PROCESS_PARAMETERS), &bytesRead)) {
        wprintf(L"Erreur de lecture du LDR (code %lu)\n", GetLastError());
        return (RTL_USER_PROCESS_PARAMETERS){0};
    }

    return params;
}



