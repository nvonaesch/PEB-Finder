#define _UNICODE
#define UNICODE
#include <windows.h>
#include <commctrl.h>
#include <wchar.h>

void addListViewRow(HWND hListView, LPCWSTR col1, LPCWSTR col2);
void addPEBField(HWND hListView, wchar_t* name, wchar_t* fmt, ...);