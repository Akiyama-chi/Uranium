#include "hider.h"
#include <windows.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <iostream>

bool Hider::hidden = false;
HWND Hider::hWnd = nullptr;

HWND GetProcessWindow(const char* windowTitle) {
    return FindWindowA(nullptr, windowTitle);
}

void Hider::ToggleHide() {
    // Always find the window in case the handle is lost after hiding
    hWnd = GetProcessWindow("Uranium | 1.0"); // Replace with actual title

    if (!hWnd) {
        MessageBoxA(0, "Failed to get ImGui Loader window handle!", "Error", MB_OK);
        return;
    }

    hidden = !hidden;
    if (hidden) {
        HideMainWindow();
        HideFromTaskManager();
        HideConsoleWindow();
    }
    else {
        ShowMainWindow();
        RestoreTaskManager();
        ShowConsoleWindow();
    }
}

void Hider::HideMainWindow() {
    if (!hWnd) return;
    ShowWindow(hWnd, SW_HIDE);
    SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
}

void Hider::ShowMainWindow() {
    // Re-fetch window handle to ensure restoration
    hWnd = GetProcessWindow("Uranium | 1.0"); // Replace with actual title
    if (!hWnd) return;

    SetWindowLongPtr(hWnd, GWL_EXSTYLE, GetWindowLongPtr(hWnd, GWL_EXSTYLE) & ~WS_EX_TOOLWINDOW);
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd); // Bring the window back to focus
}

void Hider::HideFromTaskManager() {
    typedef NTSTATUS(WINAPI* _NtSetInformationProcess)(HANDLE, PROCESS_INFORMATION_CLASS, PVOID, ULONG);

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return;

    _NtSetInformationProcess NtSetInformationProcess =
        (_NtSetInformationProcess)GetProcAddress(hNtdll, "NtSetInformationProcess");

    if (NtSetInformationProcess) {
        DWORD dwEnable = 1;
        NtSetInformationProcess(GetCurrentProcess(), (PROCESS_INFORMATION_CLASS)0x1D, &dwEnable, sizeof(dwEnable));
    }
}

void Hider::RestoreTaskManager() {
    typedef NTSTATUS(WINAPI* _NtSetInformationProcess)(HANDLE, PROCESS_INFORMATION_CLASS, PVOID, ULONG);

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    if (!hNtdll) return;

    _NtSetInformationProcess NtSetInformationProcess =
        (_NtSetInformationProcess)GetProcAddress(hNtdll, "NtSetInformationProcess");

    if (NtSetInformationProcess) {
        DWORD dwEnable = 0;
        NtSetInformationProcess(GetCurrentProcess(), (PROCESS_INFORMATION_CLASS)0x1D, &dwEnable, sizeof(dwEnable));
    }
}

void Hider::HideConsoleWindow() {
    HWND console = GetConsoleWindow();
    if (console) ShowWindow(console, SW_HIDE);
}

void Hider::ShowConsoleWindow() {
    HWND console = GetConsoleWindow();
    if (console) ShowWindow(console, SW_SHOW);
}
