#include "dxscreen.h"

#include <windows.h>


HMODULE GetSystemModule(const char* module) {
    static std::string systemPath;
    if (systemPath.empty()) {
        systemPath.resize(2048);
        uint32_t res = GetSystemDirectoryA(&systemPath[0], systemPath.size());
        systemPath.resize(res);
    }

    std::string basePath = systemPath + "\\" + module;
    return GetModuleHandleA(basePath.c_str());
}

QByteArray PackImageData(NQtScreen::EImgByteFormat fmt,
                         char* intarray,
                         unsigned rows,
                         unsigned columns)
{
    QByteArray data;
    if (fmt == NQtScreen::BF_B8G8R8A8 || fmt == NQtScreen::BF_R8G8B8A8) {
        data.resize(1 + 4 + 4 + rows * columns * 4);
        memcpy(data.data() + 1 + 4 + 4, intarray, rows * columns * 4);
    } else {
        data.resize(1 + 4 + 4 + rows * columns * 3);
        memcpy(data.data() + 1 + 4 + 4, intarray, rows * columns * 3);
    }
    data[0] = fmt;
    *(uint32_t*)(data.data() + 1) = (uint32_t)rows;
    *(uint32_t*)(data.data() + 1 + 4) = (uint32_t)columns;
    return data;
}
