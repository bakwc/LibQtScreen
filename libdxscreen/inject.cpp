#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

#include "inject.h"

int GetProcessID(const std::wstring &exeFileName) {
    HANDLE snapHandle;
    PROCESSENTRY32 processEntry = {0};

    if ((snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
        return 0;
    }

    processEntry.dwSize = sizeof(PROCESSENTRY32);
    Process32First(snapHandle, &processEntry);
    do {
        if (wcscmp(processEntry.szExeFile, exeFileName.c_str()) == 0 ) {
            return processEntry.th32ProcessID;
        }
    } while (Process32Next(snapHandle, &processEntry));

    if (snapHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(snapHandle);
    }

    return 0;
}

bool InjectDll(int pid, const std::string& dll) {

    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    HMODULE hKernel32 = ::GetModuleHandle(L"kernel32.dll");

    void* remoteMemoryBlock = ::VirtualAllocEx(hProcess, NULL, dll.size() + 1, MEM_COMMIT, PAGE_READWRITE );

    if (!remoteMemoryBlock) {
        return false;
    }

    ::WriteProcessMemory(hProcess, remoteMemoryBlock, (void*)dll.c_str(), dll.size() + 1, NULL);

    HANDLE hThread = ::CreateRemoteThread(hProcess, NULL, 0,
                    (LPTHREAD_START_ROUTINE)::GetProcAddress(hKernel32, "LoadLibraryA"),
                    remoteMemoryBlock, 0, NULL);

    if (hThread == NULL ) {
        ::VirtualFreeEx(hProcess, remoteMemoryBlock, dll.size(), MEM_RELEASE);
        return false;
    }

    return true;
}
