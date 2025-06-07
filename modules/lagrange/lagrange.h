#pragma once
#include "../../Utils/MemoWriter.h"
#include <vector>
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>
#include <iostream>

class LagRange {
public:
    LagRange();
    void startLagRangeThread();

private:
    bool isLagRangeActive = false; ///for separate original opcodes
    MemoWriter process;
    uintptr_t lagRangeAddress = 0;
    std::vector<BYTE> originalLagRangeOpCodes;
    void lagRangeLoop();
    void updateLagRange(bool enable);
};

namespace LagRangeMouseHook {
    inline HHOOK hookdoMouse;
    inline bool mouseDown = false;
    inline bool rightMouseDown = false;

    inline LRESULT __stdcall mouse_callback(int code, WPARAM wparam, LPARAM lparam) {
        if (code < 0) {
            return CallNextHookEx(hookdoMouse, code, wparam, lparam);
        }

        MSLLHOOKSTRUCT* hook = (MSLLHOOKSTRUCT*)lparam;

        if ((hook->flags == LLMHF_INJECTED) || (hook->flags == LLMHF_LOWER_IL_INJECTED)) {
            return CallNextHookEx(hookdoMouse, code, wparam, lparam);
        }

        if (wparam == WM_LBUTTONDOWN) {
            mouseDown = true;
        }
        else if (wparam == WM_LBUTTONUP) {
            mouseDown = false;
        }
        else if (wparam == WM_RBUTTONDOWN) {
            rightMouseDown = true;
        }
        else if (wparam == WM_RBUTTONUP) {
            rightMouseDown = false;
        }

        return CallNextHookEx(hookdoMouse, code, wparam, lparam);
    }

    inline DWORD WINAPI hookmouse() {
        hookdoMouse = SetWindowsHookEx(WH_MOUSE_LL, &mouse_callback, NULL, 0);
        MSG msg;

        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        UnhookWindowsHookEx(hookdoMouse);
        return 0;
    }

    inline void StartMouseHook() {
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)hookmouse, 0, 0, 0);
    }

    inline bool IsMouseDown() {
        return mouseDown;
    }
}
