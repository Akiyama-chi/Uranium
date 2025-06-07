#pragma once
#include "../../Utils/MemoWriter.h"
#include <vector>
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>

class Blink {
public:
    Blink();
    void startBlinkThread();

private:
    MemoWriter process;
    uintptr_t blinkAddress = 0;
    std::vector<BYTE> originalBlinkOpCodes;
    void blinkLoop();
    void updateBlink(bool enable);
};