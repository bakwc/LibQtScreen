#include <QImage>
#include <functional>
#include <windows.h>

using TScreenCallback = std::function<void(const QImage&)>;

HMODULE GetSystemModule(const char* module);

enum EImgByteFormat {
    BF_R8G8B8,
    BF_B8G8R8,
};

QImage IntArrayToQImage(EImgByteFormat fmt,
                        char* intarray,
                        unsigned rows,
                        unsigned columns);

void MakeDX8Screen(const TScreenCallback& callback,
                   uint64_t presentOffset);

void MakeDX9Screen(const TScreenCallback& callback,
                   uint64_t presentOffset,
                   uint64_t presentExOffset);

void MakeDXGIScreen(const TScreenCallback& callback,
                    uint64_t dxgiOffset);
