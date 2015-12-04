#include <windows.h>
#include <thread>
#include <iostream>

#include <QEventLoop>
#include <QCoreApplication>

#include "injected_app.h"

void MainLoop() {
    // Check if this is a Qt application
    if (QCoreApplication::instance()) {
        QEventLoop loop;
        TInjectedApp myApp;
        loop.exec();
    } else {
        int argc = 0;
        char** argv = nullptr;
        QCoreApplication loop(argc, argv);
        TInjectedApp myApp;
        loop.exec();
    }
}

extern "C"
{

__declspec (dllexport) BOOL __stdcall DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        std::thread thread(&MainLoop);
        thread.detach();
    } break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

}
