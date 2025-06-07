#include "spoofdid.h"
#include <random>
#include <sstream>
#include <iostream>

std::string GenerateRandomHex(size_t length) {
    const char hexChars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 15);

    std::stringstream ss;
    for (size_t i = 0; i < length; i++) {
        ss << hexChars[distrib(gen)];
    }
    return ss.str();
}

std::string GenerateRandomDeviceID(const std::string& originalID) {
    std::vector<size_t> segmentLengths;
    std::stringstream ss(originalID);
    std::string segment, randomizedID;

    // Extract segment lengths
    while (std::getline(ss, segment, '-')) {
        segmentLengths.push_back(segment.size());
    }

    // Generate a new randomized Device ID
    for (size_t i = 0; i < segmentLengths.size(); i++) {
        if (i > 0) randomizedID += "-";
        randomizedID += GenerateRandomHex(segmentLengths[i]);
    }

    return randomizedID;
}
