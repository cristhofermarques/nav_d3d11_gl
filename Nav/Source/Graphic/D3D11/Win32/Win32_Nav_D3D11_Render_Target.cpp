#include <Graphic/D3D11/Nav_D3D11_Render_Target.hpp>
#include <Graphic/D3D11/Win32/Win32_Nav_D3D11_Render_Target_Platform_Part.hpp>
#include <Graphic/D3D11/Win32/Win32_Nav_D3D11_Context_Platform_Part.hpp>
#include <UI/Window/Win32/Win32_Nav_Window_Platform_Part.hpp>
#include <Debug/Nav_Log.hpp>

IMPLEMENT_GET_PLATFORM_PART(D3D11RenderTarget);
IMPLEMENT_PLATFORM_PART_GET_SIZE(D3D11RenderTarget);

Bool D3D11RenderTarget::Create(D3D11Context::PlatformPart* ctxPlatPart, D3D11RenderTarget* renderTarget)
{
    if(ctxPlatPart == NullPtr || renderTarget == NullPtr || ctxPlatPart->d3d11Device == NullPtr){return False;}

    PlatformPart* platPart = renderTarget->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    IDXGIDevice* dxgiDevice = NullPtr;
    if(FAILED(ctxPlatPart->d3d11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice))){return False;}

    IDXGIAdapter* dxgiAdapter = NullPtr;
    if(FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
    {
        dxgiDevice->Release();
        dxgiDevice = NullPtr;
        return False;
    }

    IDXGIFactory* dxgiFactory = NullPtr;
    if(FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
    {
        dxgiAdapter->Release();
        dxgiDevice->Release();
        dxgiAdapter = NullPtr;
        dxgiDevice = NullPtr;
        return False;
    }

    platPart->dxgiDevice = dxgiDevice;
    platPart->dxgiAdapter = dxgiAdapter;
    platPart->dxgiFactory = dxgiFactory;

    return True;
};

Bool D3D11RenderTarget::Destroy(D3D11RenderTarget* renderTarget)
{
    PlatformPart* platPart = renderTarget->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    if(platPart->renderTargetView != NullPtr)
    {
        platPart->renderTargetView->Release();
        platPart->renderTargetView = NullPtr;
    }

    if(platPart->swapChain != NullPtr)
    {
        platPart->swapChain->Release();
        platPart->swapChain = NullPtr;
    }

    if(platPart->dxgiFactory != NullPtr)
    {
        platPart->dxgiFactory->Release();
        platPart->dxgiFactory = NullPtr;
    }

    if(platPart->dxgiAdapter != NullPtr)
    {
        platPart->dxgiAdapter->Release();
        platPart->dxgiAdapter = NullPtr;
    }

    if(platPart->dxgiDevice != NullPtr)
    {
        platPart->dxgiDevice->Release();
        platPart->dxgiDevice = NullPtr;
    }

    return True;
}

Bool D3D11RenderTarget::BindWindow(Window::PlatformPart* wndPlatPart, Int2 wndSize, Bool wndIsWindowed, D3D11Context::PlatformPart* d3d11CtxPlatPart, UInt32 refreshRateNumerator, UInt32 refreshRateDenominator, UInt32 sampleCount, UInt32 sampleQuality, UInt32 bufferCount)
{
    PlatformPart* platPart = this->GetPlatformPart();
    if(wndPlatPart == NullPtr || platPart == NullPtr || platPart->dxgiFactory == NullPtr || d3d11CtxPlatPart == NullPtr || d3d11CtxPlatPart->d3d11Device == NullPtr){return False;}

    DXGI_SWAP_CHAIN_DESC swapChainDesc = NullStruct;
    swapChainDesc.BufferDesc.Width = wndSize.x;
    swapChainDesc.BufferDesc.Height = wndSize.y;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = refreshRateNumerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = refreshRateDenominator;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = sampleCount;
    swapChainDesc.SampleDesc.Quality = sampleQuality;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.OutputWindow = wndPlatPart->hWnd;
    swapChainDesc.Windowed = wndIsWindowed;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain* swapChain = NullPtr;
    if(FAILED((platPart->dxgiFactory->CreateSwapChain(d3d11CtxPlatPart->d3d11Device, &swapChainDesc, &swapChain)))){return False;}

    platPart->swapChain = swapChain;
    platPart->swapChainDesc = swapChainDesc;
    
    return True;
}

Bool D3D11RenderTarget::BindContext(D3D11Context::PlatformPart* d3d11PlatPart)
{
    if(d3d11PlatPart == NullPtr || d3d11PlatPart->d3d11Device == NullPtr|| d3d11PlatPart->d3d11DeviceContext == NullPtr){return False;}

    PlatformPart* platPart = this->GetPlatformPart();
    if(platPart == NullPtr || platPart->swapChain == NullPtr){return False;}

    ID3D11Texture2D* backBuffer;
    if(FAILED(platPart->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer))){return False;}

    ID3D11RenderTargetView* renderTargetView = NullPtr;
    if(FAILED(d3d11PlatPart->d3d11Device->CreateRenderTargetView(backBuffer, Null, &renderTargetView))){return False;}

    d3d11PlatPart->d3d11DeviceContext->OMSetRenderTargets(1, &renderTargetView, NullPtr);

    platPart->renderTargetView = renderTargetView;

    return True;
}