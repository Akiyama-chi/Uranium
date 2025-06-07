#pragma once
#include "../../Utils/MemoWriter.h"
#include <vector>
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>

class VelocityY {
public:
    VelocityY();
    void startVelocityYThread();

private:
    MemoWriter process;
    uintptr_t velocityYAddress = 0;
    std::vector<BYTE> originalOpYCodes;
    void velocityYLoop();
    void updateVelocityY(bool enable);
};