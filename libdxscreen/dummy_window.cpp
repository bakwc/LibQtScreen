#include "dummy_window.h"

#include <windows.h>
#include <iostream>

bool RegisterDummyWindow() {
    static bool registered = false;
    if (registered) {
        return true;
    }

    WNDCLASSA wc;

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_OWNDC;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = DUMMY_WINDOW;

    if (!RegisterClassA(&wc)) {
        std::cerr << "failed to register dummy class\n";
        return false;
    }

    return true;
}
