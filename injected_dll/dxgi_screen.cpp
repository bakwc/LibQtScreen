#include "dxscreen.h"
#include "hook.h"

#include <windows.h>
#include <d3d10.h>
#include <dxgi.h>

void GetDX10Screenshot(IDXGISwapChain* swapChain, QByteArray& screen);
void GetDX11Screenshot(IDXGISwapChain* swapChain, QByteArray& screen);

struct THookCtx {
    void* PresentFun = nullptr;
    TScreenCallback Callback;
};
static THookCtx* HookCtx = new THookCtx();

static HRESULT STDMETHODCALLTYPE HookPresent(IDXGISwapChain* swapChain,
                                             UINT syncInterval, UINT flags)
{
    UnHook(HookCtx->PresentFun);
    QByteArray screen;
    GetDX10Screenshot(swapChain, screen);
    if (screen.isEmpty()) {
        GetDX11Screenshot(swapChain, screen);
    }
    if (!screen.isEmpty()) {
        HookCtx->Callback(screen);
    }
    return swapChain->Present(syncInterval, flags);
}

bool MakeDXGIScreen(const TScreenCallback& callback,
                    uint64_t dxgiOffset)
{
    HMODULE dxgiModule = GetSystemModule("dxgi.dll");
    if (!dxgiModule) {
        return false;
    }
    HookCtx->Callback = callback;
    HookCtx->PresentFun = (void*)((uintptr_t)dxgiModule + (uintptr_t)dxgiOffset);
    Hook(HookCtx->PresentFun, HookPresent);
    return true;
}
