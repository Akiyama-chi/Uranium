#ifdef SpoofString
#undef SpoofString
#endif
#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include <windows.h>
#include "MemoWriter.h"
#include <algorithm>  // Required for std::min
#include <fstream>

//This is for logging purposes, so that users are able to send the file to me if they find a bug.
// Logging function
void LogMessage(const std::string& message) {
    std::ofstream logFile("MemoWriter.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

MemoWriter::MemoWriter(const char* processName) : processName(processName), processID(0), hProcess(NULL) {}

MemoWriter::~MemoWriter() {
    if (hProcess) {
        CloseHandle(hProcess);
    }
}

DWORD MemoWriter::GetProcessIDByName(const char* processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (_stricmp(pe32.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

bool MemoWriter::AttachProcess() {
    if (hProcess) {
        CloseHandle(hProcess);
        hProcess = NULL;
    }

    processID = GetProcessIDByName(processName);
    if (processID == 0) {
        return false;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    return hProcess != NULL;
}

uintptr_t MemoWriter::GetModuleBaseAddress(const char* moduleName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    uintptr_t moduleBase = 0;
    if (Module32First(hSnapshot, &me32)) {
        do {
            if (_stricmp(me32.szModule, moduleName) == 0) {
                moduleBase = (uintptr_t)me32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &me32));
    }

    CloseHandle(hSnapshot);
    return moduleBase;
}

bool MemoWriter::WriteMemory(uintptr_t address, const BYTE* data, size_t size) {
    if (!hProcess || hProcess == INVALID_HANDLE_VALUE) return false;

    DWORD oldProtect;
    if (!VirtualProtectEx(hProcess, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }

    SIZE_T bytesWritten;
    BOOL success = WriteProcessMemory(hProcess, (LPVOID)address, data, size, &bytesWritten);
    VirtualProtectEx(hProcess, (LPVOID)address, size, oldProtect, &oldProtect);

    return success && bytesWritten == size;
}

bool MemoWriter::ReadMemory(uintptr_t address, BYTE* buffer, size_t size) {
    if (!hProcess || hProcess == INVALID_HANDLE_VALUE) return false;

    SIZE_T bytesRead;
    BOOL success = ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, &bytesRead);

    return success && bytesRead == size;
}

std::vector<uintptr_t> MemoWriter::FindStringInMemory(const std::string& target) {
    std::vector<uintptr_t> addresses;
    if (!hProcess || hProcess == INVALID_HANDLE_VALUE) return addresses;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    uintptr_t startAddress = (uintptr_t)sysInfo.lpMinimumApplicationAddress;
    uintptr_t endAddress = (uintptr_t)sysInfo.lpMaximumApplicationAddress;

    MEMORY_BASIC_INFORMATION mbi;
    constexpr SIZE_T bufferSize = 65536; // Increase buffer size for efficiency
    std::vector<char> buffer(bufferSize * 2);

    for (uintptr_t address = startAddress; address < endAddress; address += mbi.RegionSize) {
        if (VirtualQueryEx(hProcess, (LPCVOID)address, &mbi, sizeof(mbi)) == 0) continue;
        if (mbi.State != MEM_COMMIT || mbi.Type == MEM_MAPPED) continue;
        if (!(mbi.Protect & PAGE_READWRITE || mbi.Protect & PAGE_WRITECOPY)) continue;

        SIZE_T bytesRead;
        uintptr_t regionEnd = address + mbi.RegionSize;

        for (uintptr_t chunk = address; chunk < regionEnd; chunk += bufferSize / 2) {
            SIZE_T toRead = std::min(bufferSize, regionEnd - chunk);
            if (!ReadProcessMemory(hProcess, (LPCVOID)chunk, buffer.data(), toRead, &bytesRead)) continue;

            std::string_view searchBuffer(buffer.data(), bytesRead);
            auto it = std::search(searchBuffer.begin(), searchBuffer.end(), target.begin(), target.end());
            while (it != searchBuffer.end()) {
                addresses.push_back(chunk + std::distance(searchBuffer.begin(), it));
                it = std::search(it + 1, searchBuffer.end(), target.begin(), target.end());
            }

            if (bytesRead == toRead) {
                std::copy(buffer.end() - target.size(), buffer.end(), buffer.begin());
            }
        }
    }

    return addresses;
}

bool MemoWriter::SpoofMemoryString(const std::string& oldString, const std::string& newString) {
    if (newString.size() > oldString.size()) {
        LogMessage("New string is too long! It must be equal or shorter than the old string.");
        return false;
    }

    std::vector<uintptr_t> addresses = FindStringInMemory(oldString);
    std::wstring oldStringW(oldString.begin(), oldString.end());
    std::vector<uintptr_t> addressesW = FindStringInMemory(
        std::string(reinterpret_cast<const char*>(oldStringW.c_str()), oldStringW.size() * 2)
    );

    addresses.insert(addresses.end(), addressesW.begin(), addressesW.end());

    if (addresses.empty()) {
        LogMessage("String not found in memory.");
        return false;
    }

    std::string paddedNewString = newString;
    paddedNewString.resize(oldString.size(), '\0');
    std::wstring newStringW(newString.begin(), newString.end());
    newStringW.resize(oldStringW.size(), L'\0');

    bool atLeastOneSuccess = false;

    for (uintptr_t address : addresses) {
        DWORD oldProtect;
        if (!VirtualProtectEx(hProcess, (LPVOID)address, paddedNewString.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            continue;
        }

        if (WriteProcessMemory(hProcess, (LPVOID)address, paddedNewString.c_str(), paddedNewString.size(), NULL)) {
            atLeastOneSuccess = true;
        }
        else {
            LogMessage("Partial modification. Some strings may remain unchanged.");
        }

        VirtualProtectEx(hProcess, (LPVOID)address, paddedNewString.size(), oldProtect, &oldProtect);
    }

    return atLeastOneSuccess;
}