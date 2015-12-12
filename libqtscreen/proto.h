#pragma once

#include "saveload.h"
#include <string>

namespace NQtScreen {

enum ECommand {
    CMD_Ping,
    CMD_Info,
    CMD_ScreenShot,
    CMD_Error,
};

enum EImgByteFormat {
    BF_R8G8B8A8,
    BF_B8G8R8A8,
    BF_R8G8B8,
};

struct TInjectedAppInfo {
    uint32_t PID = 0;
    std::string Name;
    bool Is64Bit = false;

    SAVELOAD(PID, Name, Is64Bit)
};

struct TInjectorHelpInfo {
    uint64_t DX8PresentOffset = 0;
    uint64_t DX9PresentOffset = 0;
    uint64_t DX9PresentExOffset = 0;
    uint64_t DXGIPresentOffset = 0;

    SAVELOAD(DX8PresentOffset, DX9PresentOffset,
             DX9PresentExOffset, DXGIPresentOffset)
};

} // NQtScreen
