#pragma once

#include "saveload.h"
#include <string>

enum ECommand {
    CMD_Ping,
    CMD_Info,
    CMD_ScreenShot,
};

struct TInjectedAppInfo {
    uint32_t PID = 0;
    std::string Name;

    SAVELOAD(PID, Name)
};

struct TInjectorHelpInfo {
    uint64_t DX8PresentOffset = 0;
    uint64_t DX9PresentOffset = 0;
    uint64_t DX9PresentExOffset = 0;
    uint64_t DXGIPresentOffset = 0;

    SAVELOAD(DX8PresentOffset, DX9PresentOffset,
             DX9PresentExOffset, DXGIPresentOffset)
};
