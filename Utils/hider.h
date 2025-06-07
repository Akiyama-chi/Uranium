#pragma once
#include <windows.h>

class Hider {
public:
    static void ToggleHide();
private:
    static bool hidden;
    static HWND hWnd;
    static void HideMainWindow();
    static void ShowMainWindow();
    static void HideFromTaskManager();
    static void RestoreTaskManager();
    
    static void HideConsoleWindow();
    static void ShowConsoleWindow();
};
