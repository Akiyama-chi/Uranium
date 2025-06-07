#pragma once
#include "../../Utils/MemoWriter.h"
#include <vector>
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>

class VelocityZ {
public:
    VelocityZ();
    void startVelocityZThread();

private:
    MemoWriter process;
    uintptr_t velocityZAddress = 0;
    std::vector<BYTE> originalOpZCodes;
    void velocityZLoop();
    void updateVelocityZ(bool enable);
};