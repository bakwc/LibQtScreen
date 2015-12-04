#include "dxscreen.h"
#include "hook.h"

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>


static DXGI_FORMAT GetDxgiFormat(DXGI_FORMAT format) {
    if (format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB) {
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    }
    if (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    }
    return format;
}

void GetDX11Screenshot(IDXGISwapChain* swapChain, QByteArray& screen) {
    IDXGIResource *backbufferPtr = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backbufferPtr);
    if (!backbufferPtr) {
        return;
    }

    ID3D11Resource* backbuffer;
    HRESULT hr = backbufferPtr->QueryInterface(__uuidof(ID3D11Resource), (void**)&backbuffer);
    if (FAILED(hr)) {
        return;
    }
    ID3D11Device* device;
    hr = swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
    if (FAILED(hr)) {
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc;
    hr = swapChain->GetDesc(&desc);
    if (FAILED(hr)) {
        return;
    }
    D3D11_TEXTURE2D_DESC textDesc = {};
    textDesc.Format = GetDxgiFormat(desc.BufferDesc.Format);
    textDesc.Width = desc.BufferDesc.Width;
    textDesc.Height = desc.BufferDesc.Height;
    textDesc.MipLevels = 1;
    textDesc.ArraySize = 1;
    textDesc.SampleDesc.Count = 1;
    textDesc.Usage = D3D11_USAGE_STAGING;
    textDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* texture = nullptr;
    hr = device->CreateTexture2D(&textDesc, 0, &texture);
    if (FAILED(hr)) {
        return;
    }

    ID3D11DeviceContext* context = nullptr;

    device->Release();

    device->GetImmediateContext(&context);
    if (!context) {
        return;
    }

    context->CopyResource(texture, backbuffer);

    D3D11_MAPPED_SUBRESOURCE mapSubres = {0, 0, 0};

    hr = context->Map(texture, 0, D3D11_MAP_READ, 0, &mapSubres);
    if (FAILED(hr)) {
        return;
    }
    EImgByteFormat fmt = BF_R8G8B8A8;
    if (textDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM ||
        textDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM ||
        textDesc.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
        textDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
        textDesc.Format == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
        textDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
    {
        fmt == BF_B8G8R8A8;
    }

    screen = PackImageData(fmt, (char*)mapSubres.pData, textDesc.Height, textDesc.Width);
}
