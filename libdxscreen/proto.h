#pragma once

#include "saveload.h"
#include <string>

enum ECommand {
    CMD_Ping,
    CMD_Info,
    CMD_ScreenShot,
};

struct TInjectedAppInfo {
    uint32_t PID;
    std::string Name;

    SAVELOAD(PID, Name)
};

struct TInjectorHelpInfo {
    uint64_t DX8PresentOffset;
    uint64_t DX9PresentOffset;
    uint64_t DX9PresentExOffset;
    uint64_t DXGIPresentOffset;

    SAVELOAD(DX8PresentOffset, DX9PresentOffset,
             DX9PresentExOffset, DXGIPresentOffset)
};
