#include "utils.h"

void addListViewRow(HWND hListView, LPCWSTR col1, LPCWSTR col2){

    LVITEMW item = {0};
    
    item.mask = LVIF_TEXT;
    item.iItem = ListView_GetItemCount(hListView);
    item.pszText = (LPWSTR)col1;

    ListView_InsertItem(hListView, &item);
    ListView_SetItemText(hListView, item.iItem, 1, (LPWSTR)col2);
}

void addPEBField(HWND hListView, wchar_t* name, wchar_t* fmt, ...){
    wchar_t buffer[64];
    va_list args;
    va_start(args, fmt);
    vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), fmt, args);
    va_end(args);
    addListViewRow(hListView, name, buffer);
}