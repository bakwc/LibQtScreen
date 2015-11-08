#include <QImage>
#include <functional>
#include <windows.h>

using TScreenCallback = std::function<void(const QImage&)>;

HMODULE GetSystemModule(const char* module);
QImage IntArrayToQImage(char* intarray, unsigned rows, unsigned columns);

void MakeDX8Screen(const TScreenCallback& callback, uint64_t presentOffset);
