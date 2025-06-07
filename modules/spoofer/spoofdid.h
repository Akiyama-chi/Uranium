#pragma once
#ifndef SPOOFDID_H
#define SPOOFDID_H

#include <string>
#include "../../Utils/MemoWriter.h"

std::string GenerateRandomDeviceID(const std::string& originalID);

#endif
