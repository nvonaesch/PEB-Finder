#define _UNICODE
#define UNICODE
#include "ui.h"
#include "peb_finder.h"
#include "utils.h"
#include <commctrl.h>

static HINSTANCE g_hInst;

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
        

        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 2) {
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

            //TODO Appel au PEB
            //TODO Afficher retour
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

            HWND hListView = CreateWindowExW(
                WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                20, 120, 325, 200,
                hwnd, (HMENU)10, g_hInst, NULL
            );

            ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

            LVCOLUMNW col = {0};
            col.mask = LVCF_TEXT | LVCF_WIDTH;
        
            col.pszText = L"Champ";
            col.cx = 160;
            ListView_InsertColumn(hListView, 0, &col);
        
            col.pszText = L"Valeur";
            col.cx = 160;
            ListView_InsertColumn(hListView, 1, &col);

            addListViewRow(hListView, L"PEB Address", L"0x7FFDF000");


        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        TextOutW(hdc, 20, 10, L"Process PID", 17);
        EndPaint(hwnd, &ps);
    } return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
