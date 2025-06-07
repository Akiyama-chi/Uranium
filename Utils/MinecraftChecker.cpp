#include "MinecraftChecker.h"
#include <Windows.h>
#include <string>

bool MinecraftChecker::IsMinecraftBedrockFocused() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) return false;

    char windowTitle[256];
    GetWindowTextA(foregroundWindow, windowTitle, sizeof(windowTitle));

    std::string title(windowTitle);
    return title.find("Minecraft") != std::string::npos; // Check if title contains "Minecraft". Hopefully, minecraft clients doesn't completely override the name "Minecraft"... I sure hope so
}
