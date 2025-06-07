#include "velocityy.h"
#include <thread>
#include <chrono>
#include <iostream>

VelocityY::VelocityY() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    velocityYAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe") + 0x32380B8;

    originalOpYCodes.resize(6);
    process.ReadMemory(velocityYAddress, originalOpYCodes.data(), originalOpYCodes.size());
}

void VelocityY::updateVelocityY(bool enable) {
    if (velocityYAddress != 0) {
        if (enable) {
            std::vector<BYTE> nopBytes(originalOpYCodes.size(), 0x90);
            process.WriteMemory(velocityYAddress, nopBytes.data(), nopBytes.size());
        }
        else {
            process.WriteMemory(velocityYAddress, originalOpYCodes.data(), originalOpYCodes.size());
        }
    }
}

void VelocityY::velocityYLoop() {
    bool lastState = false;

    while (true) {
        if (process.GetProcessID()) {
            bool enableVelocityY = utilities::velocityy;

            if (enableVelocityY != lastState) {
                updateVelocityY(enableVelocityY);
                lastState = enableVelocityY;
            }
        }
        Sleep(50);
    }
}

void VelocityY::startVelocityYThread() {
    std::thread(&VelocityY::velocityYLoop, this).detach();
}
