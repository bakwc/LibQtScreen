#include "../libqtscreen/proto.h"

#include <QByteArray>
#include <functional>
#include <windows.h>


using TScreenCallback = std::function<void(const QByteArray&)>;

HMODULE GetSystemModule(const char* module);


QByteArray PackImageData(NQtScreen::EImgByteFormat fmt,
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
