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
    if (fmt == BF_B8G8R8A8 || fmt == BF_B8G8R8A8) {
        for (unsigned row = rows; row; row--) {
            for (unsigned col = 0; col < columns; col++) {
                char red, green, blue;
                int idx = (int(row) - 1) * columns * 4 + col * 4;
                if (fmt == BF_R8G8B8A8) {
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
    } else {
        for (int j = 0; j < rows; j++) {
            for (int i = 0; i < columns; i++) {
                int offset = (i + j * columns) * (3 * sizeof(uint8_t));

                uint8_t r = intarray[offset + 0];
                uint8_t g = intarray[offset + 1];
                uint8_t b = intarray[offset + 2];

                resImg.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }
    return resImg;
}
