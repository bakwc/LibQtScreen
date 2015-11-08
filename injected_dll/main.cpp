#include <windows.h>
#include <thread>
#include <iostream>

#include "injected_app.h"

void MainLoop() {
    int argc = 0;
    char **argv = nullptr;
    TInjectedApp app(argc, argv);
    app.exec();
}

extern "C"
{

__declspec (dllexport) BOOL __stdcall DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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
