#ifndef __WIN32_NAV_D3D11_RENDER_TARGET_PLATFORM_PART_HPP__
#define __WIN32_NAV_D3D11_RENDER_TARGET_PLATFORM_PART_HPP__

#include <Graphic/D3D11/Nav_D3D11_Render_Target.hpp>
#include <dxgi.h>
#include <d3d11.h>

IMPLEMENT_PLATFORM_PART(D3D11RenderTarget)
{
    IDXGIDevice* dxgiDevice = NullPtr;
    IDXGIAdapter* dxgiAdapter = NullPtr;
    IDXGIFactory* dxgiFactory = NullPtr;
    IDXGISwapChain* swapChain = NullPtr;
    ID3D11RenderTargetView* renderTargetView = NullPtr;
    DXGI_SWAP_CHAIN_DESC swapChainDesc = NullStruct;
};

#endif