#pragma once

#include <string>
#include <windows.h>

class TDummyWindow {
public:
    TDummyWindow(const std::string& caption);
    ~TDummyWindow();
    operator HWND();
    operator bool();
private:
    std::string Caption;
    HWND Hwnd;
};
