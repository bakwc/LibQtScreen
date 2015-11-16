#include "dxscreen.h"

#include <windows.h>
#include <d3d10.h>
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

void GetDX10Screenshot(IDXGISwapChain* swapChain, QImage& screenShotImg) {
    IDXGIResource *backbufferPtr = nullptr;
    swapChain->GetBuffer(0, __uuidof(ID3D10Resource), (void**)&backbufferPtr);
    if (!backbufferPtr) {
        return;
    }

    ID3D10Resource* backbuffer;
    HRESULT hr = backbufferPtr->QueryInterface(__uuidof(ID3D10Resource), (void**)&backbuffer);
    if (FAILED(hr)) {
        return;
    }
    ID3D10Device* device;
    hr = swapChain->GetDevice(__uuidof(ID3D10Device), (void**)&device);
    if (FAILED(hr)) {
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc;
    hr = swapChain->GetDesc(&desc);
    if (FAILED(hr)) {
        return;
    }
    D3D10_TEXTURE2D_DESC textDesc = {};
    textDesc.Format = GetDxgiFormat(desc.BufferDesc.Format);
    textDesc.Width = desc.BufferDesc.Width;
    textDesc.Height = desc.BufferDesc.Height;
    textDesc.MipLevels = 1;
    textDesc.ArraySize = 1;
    textDesc.SampleDesc.Count = 1;
    textDesc.Usage = D3D10_USAGE_STAGING;
    textDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;

    ID3D10Texture2D* texture = nullptr;
    hr = device->CreateTexture2D(&textDesc, 0, &texture);
    if (FAILED(hr)) {
        return;
    }

    device->CopyResource(texture, backbuffer);

    D3D10_MAPPED_TEXTURE2D mapText = {0, 0};
    hr = texture->Map(0, D3D10_MAP_READ, 0, &mapText);
    if (FAILED(hr)) {
        return;
    }

    EImgByteFormat fmt = BF_R8G8B8;
    if (textDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM ||
        textDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM ||
        textDesc.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
        textDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
        textDesc.Format == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
        textDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
    {
        fmt == BF_B8G8R8;
    }

    screenShotImg = IntArrayToQImage(fmt, (char*)mapText.pData, textDesc.Height, textDesc.Width);
}

