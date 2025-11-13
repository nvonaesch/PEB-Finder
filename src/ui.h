#pragma once
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

int RunMainWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
