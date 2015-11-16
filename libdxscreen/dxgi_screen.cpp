#include "dxscreen.h"
#include "hook.h"

#include <windows.h>
#include <d3d10.h>
#include <dxgi.h>

void GetDX10Screenshot(IDXGISwapChain* swapChain, QImage& screenShotImg);
void GetDX11Screenshot(IDXGISwapChain* swapChain, QImage& screenShotImg);

struct THookCtx {
    void* PresentFun = nullptr;
    TScreenCallback Callback;
};
static THookCtx* HookCtx = new THookCtx();

static HRESULT STDMETHODCALLTYPE HookPresent(IDXGISwapChain* swapChain,
                                             UINT syncInterval, UINT flags)
{
    UnHook(HookCtx->PresentFun);
    QImage screenShotImg;
    GetDX10Screenshot(swapChain, screenShotImg);
    if (screenShotImg.isNull()) {
        GetDX11Screenshot(swapChain, screenShotImg);
    }
    if (!screenShotImg.isNull()) {
        HookCtx->Callback(screenShotImg);
    }
    return swapChain->Present(syncInterval, flags);
}

void MakeDXGIScreen(const TScreenCallback& callback,
                    uint64_t dxgiOffset)
{
    HMODULE dxgiModule = GetSystemModule("dxgi.dll");
    if (!dxgiModule) {
        return;
    }
    HookCtx->Callback = callback;
    HookCtx->PresentFun = (void*)((uintptr_t)dxgiModule + (uintptr_t)dxgiOffset);
    Hook(HookCtx->PresentFun, HookPresent);
}
