#pragma once
#include "../../Utils/MemoWriter.h"
#include <vector>
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>

class NoFall {
public:
    NoFall();
    void startNoFallThread();

private:
    MemoWriter process;
    uintptr_t noFallAddress = 0;
    std::vector<BYTE> originalOpCodes;

    void noFallLoop();
    void updateNoFall(bool enable);
};
