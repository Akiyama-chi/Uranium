#pragma once
#include "../../Utils/MemoWriter.h"
#include <vector>
#include <thread>
#include "../../ui/ui.hh"
#include <Windows.h>

class Glide {
public:
    Glide();
    void startGlideThread();

private:
    MemoWriter process;
    uintptr_t glideAddress = 0;
    std::vector<BYTE> originalGlideOpCodes;
    void glideLoop();
    void updateGlide(bool enable);
};