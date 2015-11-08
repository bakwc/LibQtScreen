#include "hook.h"

#include <windows.h>
#include <stdint.h>
#include <map>
#include <string>

static std::map<uint64_t, std::string> OldData;

void DoHook(void *func, void *handler) {
    uint8_t* origFunc = (uint8_t*)func;
    uint8_t* newFunc = (uint8_t*)handler;
    DWORD t;
    VirtualProtect(origFunc, 5, PAGE_EXECUTE_READWRITE, &t);
    uint32_t distance = newFunc - origFunc - 5;
    std::string oldData;
    oldData.resize(5);
    memcpy(&oldData[0], origFunc, 5);
    *origFunc = 0xE9;
    *(uint32_t*)(origFunc + 1) = distance;
    OldData[(uint64_t)origFunc] = oldData;
}

void DoUnHook(void *func) {
    uint8_t* origFunc = (uint8_t*)func;
    std::string oldData = OldData[(uint64_t)origFunc];
    memcpy(origFunc, &oldData[0], 5);
    OldData.erase((uint64_t)origFunc);
}
