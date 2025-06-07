#pragma once
#include "../../Utils/MemoWriter.h"
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>

class Reach {
public:
    Reach();
    void startReachThread();

private:
    MemoWriter process;
    uintptr_t reachAddress = 0;
    float originalReachValue = 3.0f;  // Default Minecraft reach distance

    void reachLoop();
    void updateReach(float reachValue);
};
