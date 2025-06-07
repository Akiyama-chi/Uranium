#include "velocityz.h"
#include <thread>
#include <chrono>
#include <iostream>

VelocityZ::VelocityZ() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    velocityZAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe") + 0x32380BD;

    originalOpZCodes.resize(4);
    process.ReadMemory(velocityZAddress, originalOpZCodes.data(), originalOpZCodes.size());
}

void VelocityZ::updateVelocityZ(bool enable) {
    if (velocityZAddress != 0) {
        if (enable) {
            std::vector<BYTE> nopBytes(originalOpZCodes.size(), 0x90);
            process.WriteMemory(velocityZAddress, nopBytes.data(), nopBytes.size());
        }
        else {
            process.WriteMemory(velocityZAddress, originalOpZCodes.data(), originalOpZCodes.size());
        }
    }
}

void VelocityZ::velocityZLoop() {
    bool lastState = false;

    while (true) {
        if (process.GetProcessID()) {
            bool velocityZAddress = utilities::velocityz;

            if (velocityZAddress != lastState) {
                updateVelocityZ(velocityZAddress);
                lastState = velocityZAddress;
            }
        }
        Sleep(50);
    }
}

void VelocityZ::startVelocityZThread() {
    std::thread(&VelocityZ::velocityZLoop, this).detach();
}
