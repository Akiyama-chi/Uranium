#include "reach.h"
#include <iostream>
#include <chrono>
#include <thread>

Reach::Reach() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    uintptr_t baseAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe");
    if (!baseAddress) {
        MessageBoxA(NULL, "Failed to find module base address.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    reachAddress = baseAddress + 0x6929FA0;

    process.ReadMemory(reachAddress, originalReachValue);
}

void Reach::updateReach(float reachValue) {
    if (reachAddress != 0) {
        process.WriteMemory(reachAddress, reachValue);
    }
}

void Reach::reachLoop() {
    float lastReachValue = originalReachValue;

    while (true) {
        if (process.GetProcessID()) {
            float newReach = (reach::C_reach && reach::reachValue != originalReachValue)
                ? static_cast<float>(reach::reachValue)
                : originalReachValue;

            if (newReach != lastReachValue) {
                updateReach(newReach);
                lastReachValue = newReach;
            }
        }
        Sleep(50);
    }
}

void Reach::startReachThread() {
    std::thread(&Reach::reachLoop, this).detach();
}
