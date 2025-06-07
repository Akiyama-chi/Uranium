#include "lagrange.h"
#include <thread>
#include <chrono>
#include <iostream>
#include "../../Utils/MinecraftChecker.h"

LagRange::LagRange() : process("Minecraft.Windows.exe") {
    if (!process.AttachProcess()) {
        MessageBoxA(NULL, "Failed to attach to Minecraft process.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    lagRangeAddress = process.GetModuleBaseAddress("Minecraft.Windows.exe") + 0x11F7566;

    originalLagRangeOpCodes.resize(8);
    process.ReadMemory(lagRangeAddress, originalLagRangeOpCodes.data(), originalLagRangeOpCodes.size());
}

void LagRange::updateLagRange(bool enable) {
    if (lagRangeAddress != 0) {
        if (enable) {
            std::vector<BYTE> nopBytes(originalLagRangeOpCodes.size(), 0x90);
            process.WriteMemory(lagRangeAddress, nopBytes.data(), nopBytes.size());
        }
        else {
            process.WriteMemory(lagRangeAddress, originalLagRangeOpCodes.data(), originalLagRangeOpCodes.size());
        }
    }
}

void LagRange::lagRangeLoop() {
    while (true) {
        if (lag::lagrange && LagRangeMouseHook::IsMouseDown() && MinecraftChecker::IsMinecraftBedrockFocused()) {  // Only execute if mouseDown is true
            if (process.GetProcessID()) {
                int cooldownMs = lag::cooldown;


                std::vector<BYTE> nopBytes(originalLagRangeOpCodes.size(), 0x90);
                process.WriteMemory(lagRangeAddress, nopBytes.data(), nopBytes.size());

                std::this_thread::sleep_for(std::chrono::milliseconds(cooldownMs));

                process.WriteMemory(lagRangeAddress, originalLagRangeOpCodes.data(), originalLagRangeOpCodes.size());

                std::this_thread::sleep_for(std::chrono::milliseconds(lag::everyms));
            }
            else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        else if(!utilities::blink && !lag::lagrange) {
            process.WriteMemory(lagRangeAddress, originalLagRangeOpCodes.data(), originalLagRangeOpCodes.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Avoid high CPU usage
        }
    }
}

void LagRange::startLagRangeThread() {
    LagRangeMouseHook::StartMouseHook(); // Start mouse hook first
    std::thread(&LagRange::lagRangeLoop, this).detach();
}
