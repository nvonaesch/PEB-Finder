#define _UNICODE
#define UNICODE
#include "ui.h"
#include "peb_loader.h"
#include "utils.h"
#include "process_listing.h"
#include <commctrl.h>

//TODO Afficher chaque thread et son état <- NtQueryInformationThread / Thread32First

static HINSTANCE g_hInst;
static HWND hListViewPeb = NULL;
static HWND hListViewLdr = NULL;
static HWND hStaticInfo = NULL;
static HWND hListViewProcess = NULL;

int RunMainWindow(HINSTANCE hInstance, int nCmdShow) {
    g_hInst = hInstance;
    const wchar_t CLASS_NAME[]  = L"PebInspector";

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    const  wchar_t lpWindowName[]  = L"PebInspector";
    HWND hwnd = CreateWindowExW(0, CLASS_NAME, lpWindowName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit;

    switch (uMsg) {
    case WM_CREATE:
        hEdit = CreateWindowExW(
            0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
            20, 40, 200, 25,
            hwnd, (HMENU)1,
            ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        CreateWindowExW(
            0, L"BUTTON", L"Ok",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            230, 40, 120, 25,
            hwnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC  = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);
        
        CreateWindowExW(
            0, L"BUTTON", L"Actualiser",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            800, 40, 120, 25,
            hwnd, (HMENU)3, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        hListViewProcess = CreateWindowExW(
                WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                WS_VSCROLL | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL ,
                700, 70, 300, 300,
                hwnd, (HMENU)10, g_hInst, NULL
        );

        ListView_SetExtendedListViewStyle(hListViewProcess, LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS );

        LVCOLUMNW col = {0};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        
        col.pszText = L"Process";
        col.cx = 110;
        ListView_InsertColumn(hListViewProcess, 0, &col);
        
        col.pszText = L"PID";
        col.cx = 80;
        ListView_InsertColumn(hListViewProcess, 1, &col);

        col.pszText = L"ParentPID";
        col.cx = 80;
        ListView_InsertColumn(hListViewProcess, 2, &col);
        ListView_SetColumnWidth(hListViewProcess, 2, LVSCW_AUTOSIZE_USEHEADER);

        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 2) {

            if (hListViewPeb) { DestroyWindow(hListViewPeb); hListViewPeb = NULL; }
            if (hListViewLdr) { DestroyWindow(hListViewLdr); hListViewLdr = NULL; }
            if (hStaticInfo)  { DestroyWindow(hStaticInfo); hStaticInfo = NULL; }

            wchar_t buffer[256];
            GetWindowTextW(hEdit, buffer, 256);

            DWORD pid = wcstoul(buffer, NULL, 10);
            HANDLE hProcess = obtainProcessHandle(pid);
            
            if(!hProcess){
                DWORD err = GetLastError();
                wchar_t msg[256];
                swprintf(msg, 256, L"Impossible d'ouvrir le PID %lu (code erreur %lu)", pid, err);
                MessageBoxW(hwnd, msg, L"Erreur", MB_ICONERROR);
                return 0;
            }

            HMODULE ntdll = loadNTDLLModule();
            NtQueryInformationProcess_t inf = obtainNtQueryInformationProcessAddress(ntdll);
            PEB peb = obtainProcessEnvironmentBlock(inf, hProcess);

            wchar_t msg[256];
            swprintf(msg, 256, L"Handle ouvert : 0x%p", hProcess);
            MessageBoxW(hwnd, msg, L"Succès", MB_OK);

            swprintf(msg, 256, L"Handle ouvert pour le PID %lu (0x%p)", pid, hProcess);

            CreateWindowExW(
                0, L"STATIC", msg,
                WS_CHILD | WS_VISIBLE,
                20, 80, 380, 25,
                hwnd, NULL, GetModuleHandleW(NULL), NULL
            );

            hListViewPeb = CreateWindowExW(
                WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                20, 120, 325, 300,
                hwnd, (HMENU)10, g_hInst, NULL
            );

            ListView_SetExtendedListViewStyle(hListViewPeb, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

            LVCOLUMNW col = {0};
            col.mask = LVCF_TEXT | LVCF_WIDTH;
        
            col.pszText = L"Champ";
            col.cx = 160;
            ListView_InsertColumn(hListViewPeb, 0, &col);
        
            col.pszText = L"Valeur";
            col.cx = 160;
            ListView_InsertColumn(hListViewPeb, 1, &col);

            ListView_SetColumnWidth(hListViewPeb, 1, LVSCW_AUTOSIZE_USEHEADER);

            addPEBField(hListViewPeb, L"InheritedAddressSpace", L"%u", peb.InheritedAddressSpace);
            addPEBField(hListViewPeb, L"ReadImageFileExecOptions", L"%u", peb.ReadImageFileExecOptions);
            addPEBField(hListViewPeb, L"BeingDebugged", L"%u", peb.BeingDebugged);
            addPEBField(hListViewPeb, L"BitField", L"%u", peb.BitField);
            addPEBField(hListViewPeb, L"Mutant", L"0x%p", peb.Mutant);
            addPEBField(hListViewPeb, L"ImageBaseAddress", L"0x%p", peb.ImageBaseAddress);
            addPEBField(hListViewPeb, L"Ldr", L"0x%p", peb.Ldr);
            addPEBField(hListViewPeb, L"ProcessParameters", L"0x%p", peb.ProcessParameters);
            addPEBField(hListViewPeb, L"Reserved4", L"0x%p", peb.Reserved4);
            addPEBField(hListViewPeb, L"AtlThunkSListPtr", L"0x%p", peb.AtlThunkSListPtr);
            addPEBField(hListViewPeb, L"Reserved5", L"0x%p", peb.Reserved5);
            addPEBField(hListViewPeb, L"Reserved6", L"%llu", peb.Reserved6);
            addPEBField(hListViewPeb, L"Reserved7", L"0x%p", peb.Reserved7);
            addPEBField(hListViewPeb, L"Reserved8", L"%llu", peb.Reserved8);
            addPEBField(hListViewPeb, L"AtlThunkSListPtr32", L"0x%p", peb.AtlThunkSListPtr32);
            addPEBField(hListViewPeb, L"PostProcessInitRoutine", L"0x%p", peb.PostProcessInitRoutine);
            addPEBField(hListViewPeb, L"SessionId", L"%llu", peb.SessionId);
            
            hListViewLdr = CreateWindowExW(
                WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                390, 20, 260, 300,
                hwnd, (HMENU)10, g_hInst, NULL
            );

            ListView_SetExtendedListViewStyle(hListViewLdr, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

            LVCOLUMNW col2 = {0};
            col2.mask = LVCF_TEXT | LVCF_WIDTH;
            col2.pszText = L"Module";
            col2.cx = 120;
            ListView_InsertColumn(hListViewLdr, 0, &col2);

            col2.pszText = L"Base Address";
            col2.cx = 130;
            ListView_InsertColumn(hListViewLdr, 1, &col2);
            ListView_SetColumnWidth(hListViewLdr, 1, LVSCW_AUTOSIZE_USEHEADER);

            showLoadedModules(hListViewLdr, hProcess, peb);

        }


        if (LOWORD(wParam) == 3) {
        //TODO Ajouter la liste process
            size_t count;
            listedProcess* pList;

            pList = listProcesses(&count);
        
            ListView_DeleteAllItems(hListViewProcess);
            
            for(int i = 0; i<(int)count;i++){
                LVITEMW item = {0};
                item.mask = LVIF_TEXT;
                item.iItem = ListView_GetItemCount(hListViewProcess);
                item.pszText = pList[i].processName;

                int index = ListView_InsertItem(hListViewProcess, &item);

                wchar_t pidStr[16];
                swprintf(pidStr, 16, L"%u", pList[i].pid);
                ListView_SetItemText(hListViewProcess, index, 1, pidStr);
                wchar_t ppidStr[16];
                swprintf(ppidStr, 16, L"%u", pList[i].parentPid);
                ListView_SetItemText(hListViewProcess, index, 2, ppidStr);
            }    
        }

        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutW(hdc, 20, 10, L"Process PID", 11);
        EndPaint(hwnd, &ps);
    } return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
