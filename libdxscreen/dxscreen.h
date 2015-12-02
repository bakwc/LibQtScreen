#include <QImage>
#include <functional>
#include <windows.h>

using TScreenCallback = std::function<void(const QImage&)>;

HMODULE GetSystemModule(const char* module);

enum EImgByteFormat {
    BF_R8G8B8A8,
    BF_B8G8R8A8,
    BF_R8G8B8,
};

QImage IntArrayToQImage(EImgByteFormat fmt,
                        char* intarray,
                        unsigned rows,
                        unsigned columns);

bool MakeDX8Screen(const TScreenCallback& callback,
                   uint64_t presentOffset);

bool MakeDX9Screen(const TScreenCallback& callback,
                   uint64_t presentOffset,
                   uint64_t presentExOffset);

bool MakeDXGIScreen(const TScreenCallback& callback,
                    uint64_t dxgiOffset);

bool MakeOpenGLScreen(const TScreenCallback& callback);
