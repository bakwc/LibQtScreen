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

QImage IntArrayToQImage(EImgByteFormat fmt,
                        char* intarray,
                        unsigned rows,
                        unsigned columns)
{
    QImage resImg(columns, rows, QImage::Format_RGB32);
    for (unsigned row = rows; row; row--) {
        for (unsigned col = 0; col < columns; col++) {
            char red, green, blue;
            int idx = (int(row) - 1) * columns * 4 + col * 4;
            if (fmt == BF_R8G8B8) {
                red = intarray[idx];
                green = intarray[idx + 1];
                blue = intarray[idx + 2];
            } else {
                blue = intarray[idx];
                green = intarray[idx + 1];
                red = intarray[idx + 2];
            }
            resImg.setPixel(col, row - 1, qRgb(red, green, blue));
        }
    }
    return resImg;
}
