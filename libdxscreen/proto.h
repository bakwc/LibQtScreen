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
    bool HasDirectX;

    SAVELOAD(PID, Name, HasDirectX)
};

struct TInjectorHelpInfo {
    uint64_t DX8PresentOffset;

    SAVELOAD(DX8PresentOffset)
};
