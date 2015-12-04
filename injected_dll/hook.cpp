#include "hook.h"

#include <windows.h>
#include <stdint.h>
#include <map>
#include <string>

static std::map<uint64_t, std::string> OldData;


bool DoHook(void *addr,void *handler) {
    if (OldData.find((uint64_t)addr) != OldData.end()) {
        return false;
    }

    if (addr != nullptr && handler != nullptr) {
        DWORD old;
        if (VirtualProtect(addr, 32, PAGE_EXECUTE_READWRITE, &old) != FALSE) {
            BYTE* code=(BYTE*)addr;
            std::string oldData;
#ifndef _WIN64
            oldData.resize(5);
            memcpy(&oldData[0], (BYTE*)addr, 5);
            *(BYTE*)&code[0] = 0xE9;
            *(int*)&code[1] = (BYTE*)handler - ((BYTE*)addr + 5);
#else
            oldData.resize(11);
            memcpy(&oldData[0], (BYTE*)addr, 11);
            *(WORD*)&code[0] = 0xB848;
            *(void**)&code[2] = handler;
            *(WORD*)&code[10] = 0xE0FF;
#endif
            OldData[(uint64_t)addr] = oldData;
            VirtualProtect(addr, 32, old, &old);
            return true;
        }
    }
    return false;
}


void DoUnHook(void *func) {
    uint8_t* origFunc = (uint8_t*)func;
    if (OldData.find((uint64_t)origFunc) == OldData.end()) {
        return;
    }
    std::string oldData = OldData[(uint64_t)origFunc];
#ifndef _WIN64
    DWORD prevProtect;
    VirtualProtect(origFunc, 5, PAGE_EXECUTE_READWRITE, &prevProtect);
    memcpy(origFunc, &oldData[0], 5);
    VirtualProtect(origFunc, 5, prevProtect, &prevProtect);
#else
    DWORD prevProtect;
    VirtualProtect(origFunc, 11, PAGE_EXECUTE_READWRITE, &prevProtect);
    memcpy(origFunc, &oldData[0], 11);
    VirtualProtect(origFunc, 11, prevProtect, &prevProtect);
#endif
    OldData.erase((uint64_t)origFunc);
}
