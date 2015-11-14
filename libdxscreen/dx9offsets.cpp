#include "dxoffsets.h"
#include "dummy_window.h"

#include <d3d9.h>
#include <windows.h>

typedef IDirect3D9 *(WINAPI *DX9CreateFunc)(UINT);

class TDirect3D9Ctx {
public:
    TDirect3D9Ctx()
        : Wnd("dx8 test window")
        , Module(0)
        , DX9(nullptr)
        , Device(nullptr)
    {
        if (!Wnd) {
            return;
        }
        Module = LoadLibraryA("d3d9.dll");
        if (!Module) {
            return;
        }
        DX9CreateFunc create = (DX9CreateFunc)GetProcAddress(Module, "Direct3DCreate9");
        if (!create) {
            return;
        }
        DX9 = create(D3D_SDK_VERSION);
        if (!DX9) {
            return;
        }
        D3DPRESENT_PARAMETERS presentParams = {};
        presentParams.Windowed = true;
        presentParams.SwapEffect = D3DSWAPEFFECT_FLIP;
        presentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
        presentParams.BackBufferWidth = 4;
        presentParams.BackBufferHeight = 4;
        presentParams.BackBufferCount = 1;
        presentParams.hDeviceWindow = Wnd;

        DX9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &presentParams, &Device);
    }
    ~TDirect3D9Ctx() {
        if (Device) {
            Device->Release();
        }
        if (DX9) {
            DX9->Release();
        }
    }
    HMODULE GetModule() {
        return Module;
    }
    LPDIRECT3DDEVICE9 GetDevice() {
        return Device;
    }
private:
    TDummyWindow Wnd;
    HMODULE Module;
    IDirect3D9* DX9;
    LPDIRECT3DDEVICE9 Device;
};


void GetDX9Offsets(uint64_t& present, uint64_t& presentEx) {
    TDirect3D9Ctx dx9;
    present = 0;
    presentEx = 0;
    if (dx9.GetModule() && dx9.GetDevice()) {
        present = GetVtableOffset((uint64_t)dx9.GetModule(), dx9.GetDevice(), 17);
        presentEx = GetVtableOffset((uint64_t)dx9.GetModule(), dx9.GetDevice(), 121);
    }
}
