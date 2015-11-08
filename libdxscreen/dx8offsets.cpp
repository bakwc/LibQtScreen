#include "dxoffsets.h"
#include "dummy_window.h"

#include <d3d8.h>
#include <windows.h>
#include <iostream>


typedef IDirect3D8 *(WINAPI *DX8CreateFunc)(UINT);

static inline bool InitDX8(HMODULE& module, LPDIRECT3DDEVICE8& device) {
    if (!RegisterDummyWindow()) {
        return false;
    }

    HMODULE hmd = GetModuleHandleA(nullptr);

    HWND hwnd = CreateWindowExA(0, DUMMY_WINDOW, "dx8 test window",
                    WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                    hmd, nullptr);
    if (!hwnd) {
        return false;
    }

    module = LoadLibraryA("d3d8.dll");
    if (!module) {
        return false;
    }

    DX8CreateFunc create = (DX8CreateFunc)GetProcAddress(module, "Direct3DCreate8");
    if (!create) {
        return false;
    }

    IDirect3D8* dx8 = create(D3D_SDK_VERSION);
    if (!dx8) {
        return false;
    }

    D3DPRESENT_PARAMETERS presentParams = {};
    presentParams.Windowed = true;
    presentParams.SwapEffect = D3DSWAPEFFECT_FLIP;
    presentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
    presentParams.BackBufferWidth = 4;
    presentParams.BackBufferHeight = 4;
    presentParams.BackBufferCount = 1;
    presentParams.hDeviceWindow = hwnd;

    dx8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                      &presentParams, &device);

    return true;
}

void GetDX8Offsets(uint64_t& present) {
    HMODULE module;
    LPDIRECT3DDEVICE8 device;
    present = 0;
    if (InitDX8(module, device)) {
        present = GetVtableOffset((uint64_t)module, device, 15);
    }
}
