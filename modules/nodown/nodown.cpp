#include "nodown.h"
#include <thread>
#include <chrono>
#include <iostream>

Glide::Glide() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    glideAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe") + 0x1E2AA4C;

    originalGlideOpCodes.resize(2);
    process.ReadMemory(glideAddress, originalGlideOpCodes.data(), originalGlideOpCodes.size());
}

void Glide::updateGlide(bool enable) {
    if (glideAddress != 0) {
        if (enable) {
            std::vector<BYTE> nopBytes(originalGlideOpCodes.size(), 0x90);
            process.WriteMemory(glideAddress, nopBytes.data(), nopBytes.size());
        }
        else {
            process.WriteMemory(glideAddress, originalGlideOpCodes.data(), originalGlideOpCodes.size());
        }
    }
}

void Glide::glideLoop() {
    bool lastState = false;

    while (true) {
        if (process.GetProcessID()) {
            bool enableGlide = utilities::glide;

            if (enableGlide != lastState) {
                updateGlide(enableGlide);
                lastState = enableGlide;
            }
        }
        Sleep(50);
    }
}

void Glide::startGlideThread() {
    std::thread(&Glide::glideLoop, this).detach();
}
