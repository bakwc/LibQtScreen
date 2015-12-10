#pragma once

#include <stdint.h>
#include <windows.h>

namespace NQtScreen {

inline uint64_t GetVtableOffset(uint64_t module, void* cls, uint32_t offset) {
    uintptr_t* virtualTable = *(uintptr_t**)cls;
    return (uint64_t)(virtualTable[offset] - module);
}

void GetDX8Offsets(uint64_t& present);
void GetDX9Offsets(uint64_t& present, uint64_t& presentEx);
void GetDXGIOffsets(uint64_t& present);

} // NQtScreen
