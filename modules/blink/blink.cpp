#include "blink.h"
#include <thread>
#include <chrono>
#include <iostream>

Blink::Blink() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    blinkAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe") + 0x11F7566;

    originalBlinkOpCodes.resize(8);
    process.ReadMemory(blinkAddress, originalBlinkOpCodes.data(), originalBlinkOpCodes.size());
}

void Blink::updateBlink(bool enable) {
    if (blinkAddress != 0) {
        if (enable) {
            std::vector<BYTE> nopBytes(originalBlinkOpCodes.size(), 0x90);
            process.WriteMemory(blinkAddress, nopBytes.data(), nopBytes.size());
        }
        else {
            process.WriteMemory(blinkAddress, originalBlinkOpCodes.data(), originalBlinkOpCodes.size());
        }
    }
}

void Blink::blinkLoop() {
    bool lastState = false;

    while (true) {
        if (process.GetProcessID()) {
            bool enableBlink = utilities::blink;

            if (enableBlink != lastState) {
                updateBlink(enableBlink);
                lastState = enableBlink;
            }
        }
        Sleep(50);
    }
}

void Blink::startBlinkThread() {
    std::thread(&Blink::blinkLoop, this).detach();
}
