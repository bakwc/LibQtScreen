#include "dummy_window.h"

#include <windows.h>

static const char* DUMMY_WINDOW = "dummy_dx_test_cls";

LRESULT CALLBACK TestProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_NCCREATE) {
        return true;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

static bool RegisterDummyWindow() {
    static bool registered = false;
    if (registered) {
        return true;
    }

    static WNDCLASSA wc;

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_OWNDC;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = TestProc;
    wc.lpszClassName = DUMMY_WINDOW;

    if (!RegisterClassA(&wc)) {
        return false;
    }

    registered = true;

    return true;
}


TDummyWindow::TDummyWindow(const std::string& caption)
    : Caption(caption)
{
    if (!RegisterDummyWindow()) {
        return;
    }
    HMODULE hmd = GetModuleHandle(nullptr);
    Hwnd = CreateWindowExA(0, DUMMY_WINDOW, Caption.c_str(),
                    WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                           hmd, nullptr);
}

TDummyWindow::~TDummyWindow() {
    if (Hwnd) {
        DestroyWindow(Hwnd);
    }
}

TDummyWindow::operator HWND() {
    return Hwnd;
}

TDummyWindow::operator bool() {
    return bool(Hwnd);
}
