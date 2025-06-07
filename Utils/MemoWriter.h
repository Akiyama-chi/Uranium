#ifndef MEMOWRITER_H
#define MEMOWRITER_H

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  // Required for std::min


class MemoWriter {
public:
    MemoWriter(const char* processName);
    ~MemoWriter();

    std::vector<uintptr_t> FindStringInMemory(const std::string& target);
    bool SpoofMemoryString(const std::string& oldString, const std::string& newString);

    bool AttachProcess();

    template <typename T>
    bool WriteMemory(uintptr_t address, const T& value);

    bool WriteMemory(uintptr_t address, const BYTE* data, size_t size);

    template <typename T>
    bool ReadMemory(uintptr_t address, T& value);

    bool ReadMemory(uintptr_t address, BYTE* buffer, size_t size);

    DWORD GetProcessID() const { return processID; }
    uintptr_t GetModuleBaseAddress(const char* moduleName);

private:
    const char* processName;
    DWORD processID;
    HANDLE hProcess;

    DWORD GetProcessIDByName(const char* processName);
};

template <typename T>
bool MemoWriter::WriteMemory(uintptr_t address, const T& value) {
    return WriteMemory(address, reinterpret_cast<const BYTE*>(&value), sizeof(T));
}

template <typename T>
bool MemoWriter::ReadMemory(uintptr_t address, T& value) {
    return ReadMemory(address, reinterpret_cast<BYTE*>(&value), sizeof(T));
}

#endif
