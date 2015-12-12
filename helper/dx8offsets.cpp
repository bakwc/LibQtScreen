#include "dxoffsets.h"
#include "dummy_window.h"

#include <d3d8.h>
#include <windows.h>

namespace NQtScreen {

typedef IDirect3D8 *(WINAPI *DX8CreateFunc)(UINT);

class TDirect3D8Ctx {
public:
    TDirect3D8Ctx()
        : Wnd("dx8 test window")
        , Module(0)
        , DX8(nullptr)
        , Device(nullptr)
    {
        if (!Wnd) {
            return;
        }

        Module = LoadLibraryA("d3d8.dll");
        if (!Module) {
            return;
        }
        DX8CreateFunc create = (DX8CreateFunc)GetProcAddress(Module, "Direct3DCreate8");
        if (!create) {
            return;
        }
        DX8 = create(D3D_SDK_VERSION);
        if (!DX8) {
            return;
        }
        D3DPRESENT_PARAMETERS presentParams = {};
        presentParams.Windowed = true;
        presentParams.SwapEffect = D3DSWAPEFFECT_FLIP;
        presentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
        presentParams.BackBufferWidth = 4;
        presentParams.BackBufferHeight = 4;
        presentParams.BackBufferCount = 1;
        presentParams.hDeviceWindow = NULL;
        presentParams.hDeviceWindow = Wnd;

        HRESULT hr = DX8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &presentParams, &Device);

    }
    ~TDirect3D8Ctx() {
        if (Device) {
            Device->Release();
        }
        if (DX8) {
            DX8->Release();
        }
    }
    HMODULE GetModule() {
        return Module;
    }
    LPDIRECT3DDEVICE8 GetDevice() {
        return Device;
    }
private:
    TDummyWindow Wnd;
    HMODULE Module;
    IDirect3D8* DX8;
    LPDIRECT3DDEVICE8 Device;
};

void GetDX8Offsets(uint64_t& present) {
    TDirect3D8Ctx dx8;
    present = 0;
    if (dx8.GetModule() && dx8.GetDevice()) {
        present = GetVtableOffset((uint64_t)dx8.GetModule(), dx8.GetDevice(), 15);
    }
}

} // NQtScreen
