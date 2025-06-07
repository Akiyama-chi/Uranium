#include "nofall.h"
#include <thread>
#include <chrono>
#include <iostream>

NoFall::NoFall() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    noFallAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe") + 0x3233060;

    originalOpCodes.resize(4);
    process.ReadMemory(noFallAddress, originalOpCodes.data(), originalOpCodes.size());
}

void NoFall::updateNoFall(bool enable) {
    if (noFallAddress != 0) {
        if (enable) {
            std::vector<BYTE> nopBytes(originalOpCodes.size(), 0x90);
            process.WriteMemory(noFallAddress, nopBytes.data(), nopBytes.size());
        }
        else {
            process.WriteMemory(noFallAddress, originalOpCodes.data(), originalOpCodes.size());
        }
    }
}

void NoFall::noFallLoop() {
    bool lastState = false;

    while (true) {
        if (process.GetProcessID()) {
            bool enableNoFall = utilities::nofall;

            if (enableNoFall != lastState) {
                updateNoFall(enableNoFall);
                lastState = enableNoFall;
            }
        }
        Sleep(50);
    }
}

void NoFall::startNoFallThread() {
    std::thread(&NoFall::noFallLoop, this).detach();
}
