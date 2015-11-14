#include "dxoffsets.h"
#include "dummy_window.h"

#include <d3d10.h>
#include <dxgi.h>
#include <windows.h>

typedef HRESULT (WINAPI *DX10CreateFunc)(IDXGIAdapter*,
                D3D10_DRIVER_TYPE, HMODULE, UINT, UINT,
                DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, IUnknown**);

class TDirect3DgiCtx {
public:
    TDirect3DgiCtx()
        : Wnd("dxgi test window")
        , DxgiModule(0)
        , Dx10Module(0)
        , Device(nullptr)
        , SwapChain(nullptr)
    {
        if (!Wnd) {
            return;
        }
        DxgiModule = LoadLibraryA("dxgi.dll");
        if (!DxgiModule) {
            return;
        }
        Dx10Module = LoadLibraryA("d3d10.dll");
        if (!Dx10Module) {
            return;
        }
        DX10CreateFunc create = (DX10CreateFunc)GetProcAddress(Dx10Module,
                                            "D3D10CreateDeviceAndSwapChain");
        if (!create) {
            return;
        }
        DXGI_SWAP_CHAIN_DESC desc = {};
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.Width = 2;
        desc.BufferDesc.Height = 2;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.OutputWindow = Wnd;
        desc.SampleDesc.Count = 1;
        desc.Windowed = true;

        create(nullptr, D3D10_DRIVER_TYPE_NULL, nullptr, 0,
                D3D10_SDK_VERSION, &desc, &SwapChain, &Device);
    }
    ~TDirect3DgiCtx() {
        if (Device) {
            Device->Release();
        }
        if (SwapChain) {
            SwapChain->Release();
        }
    }
    HMODULE GetDxgiModule() {
        return DxgiModule;
    }
    IDXGISwapChain* GetSwapChain() {
        return SwapChain;
    }
private:
    TDummyWindow Wnd;
    HMODULE DxgiModule;
    HMODULE Dx10Module;
    IUnknown* Device;
    IDXGISwapChain* SwapChain;
};

void GetDXGIOffsets(uint64_t& present) {
    TDirect3DgiCtx dgi;
    present = 0;
    if (dgi.GetSwapChain() && dgi.GetDxgiModule()) {
        present = GetVtableOffset((uint64_t)dgi.GetDxgiModule(), dgi.GetSwapChain(), 8);
    }
}
