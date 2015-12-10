#include <string>
#include <iostream>
#include <QTimer>

#include "dxoffsets.h"
#include "inject.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        return 1;
    }

    std::string command = argv[1];
    if (command == "offsets") {
        uint64_t dx8Present;
        uint64_t dx9Present, dx9PresentEx;
        uint64_t dxGIPresent;
        new QTimer();
        NQtScreen::GetDX8Offsets(dx8Present);
        NQtScreen::GetDX9Offsets(dx9Present, dx9PresentEx);
        NQtScreen::GetDXGIOffsets(dxGIPresent);
        std::cout << dx8Present << "\n";
        std::cout << dx9Present << "\n";
        std::cout << dx9PresentEx << "\n";
        std::cout << dxGIPresent << "\n";
        return 0;
    }
    if (command == "inject") {
        if (argc < 4) {
            return 2;
        }
        int pid = std::stoi(std::string(argv[2]));
        std::string dllPath = std::string(argv[3]);
        if (NQtScreen::InjectDll(pid, dllPath)) {
            return 0;
        }
        return 3;
    }
    return 4;
}
