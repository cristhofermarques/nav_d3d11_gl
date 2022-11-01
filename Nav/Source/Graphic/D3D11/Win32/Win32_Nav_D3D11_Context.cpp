#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <UI/Window/Win32/Win32_Nav_Window_Platform_Part.hpp>
#include <Graphic/D3D11/Win32/Win32_Nav_D3D11_Context_Platform_Part.hpp>

IMPLEMENT_GET_PLATFORM_PART(D3D11Context);
IMPLEMENT_PLATFORM_PART_GET_SIZE(D3D11Context);

Bool D3D11Context::Bind(Window::PlatformPart* wndPlatPart, Int2 wndSize, Bool wndIsWindowed, UInt32 refreshRateNumerator, UInt32 refreshRateDenominator, UInt32 sampleCount, UInt32 sampleQuality, UInt32 bufferCount)
{
    if(_isBinded){return False;}
    PlatformPart* platPart = this->GetPlatformPart();
    if(wndPlatPart == NullPtr || wndPlatPart->hWnd == Null || platPart == NullPtr || platPart->dxgiFactory == NullPtr || platPart->d3d11Device == NullPtr || platPart->d3d11DeviceContext == NullPtr){return False;}

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
    if(FAILED((platPart->dxgiFactory->CreateSwapChain(platPart->d3d11Device, &swapChainDesc, &swapChain)))){return False;}

    platPart->swapChain = swapChain;
    platPart->swapChainDesc = swapChainDesc;

    ID3D11Texture2D* backBuffer;
    if(FAILED(platPart->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer))){return False;}

    ID3D11RenderTargetView* renderTargetView = NullPtr;
    if(FAILED(platPart->d3d11Device->CreateRenderTargetView(backBuffer, Null, &renderTargetView))){return False;}

    platPart->d3d11DeviceContext->OMSetRenderTargets(1, &renderTargetView, NullPtr);

    platPart->renderTargetView = renderTargetView;
    
    _isBinded = True;
    return True;
}

Bool D3D11Context::Unbind()
{
    if(!_isBinded){return False;}
    PlatformPart* platPart = this->GetPlatformPart();
    if(platPart == NullPtr || platPart->swapChain == NullPtr || platPart->renderTargetView == NullPtr){return False;}

    platPart->renderTargetView->Release();
    platPart->swapChain->Release();

    platPart->renderTargetView = NullPtr;
    platPart->swapChain = NullPtr;
    platPart->swapChainDesc = NullStruct;

    _isBinded = False;
    return True;
}

void D3D11Context::Clear(Float4 color)
{
    if(_isBinded)
    {
        PlatformPart* platPart = GetPlatformPart();
        platPart->d3d11DeviceContext->ClearRenderTargetView(platPart->renderTargetView, &color.x);
    }
}

void D3D11Context::Present(VSyncMode vSyncMode)
{
    if(_isBinded)
    {
        PlatformPart* platPart = GetPlatformPart();
        platPart->swapChain->Present((UINT)vSyncMode, Null);
        platPart->d3d11DeviceContext->OMSetRenderTargets(1, &platPart->renderTargetView, NullPtr);
    }
}

Bool D3D11Context::Create(D3D11Context* ctx)
{
    if(ctx == NullPtr){return False;}

    ctx->_isBinded = False;

    PlatformPart* platPart = ctx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    *platPart = NullStruct;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };
    UINT featureLevelsLen = ARRAYSIZE(featureLevels);

    if FAILED(D3D11CreateDevice(Null, D3D_DRIVER_TYPE_HARDWARE, Null, Null, featureLevels, featureLevelsLen, D3D11_SDK_VERSION, &platPart->d3d11Device, &platPart->featureLevel, &platPart->d3d11DeviceContext)){return False;}

    IDXGIDevice* dxgiDevice = NullPtr;
    if(FAILED(platPart->d3d11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice))){return False;}

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
}

Bool D3D11Context::Destroy(D3D11Context* ctx)
{
    if(ctx == NullPtr){return False;}

    PlatformPart* platPart = ctx->GetPlatformPart();
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

    if(platPart->d3d11DeviceContext != NullPtr)
    {
        platPart->d3d11DeviceContext->Release();
        platPart->d3d11DeviceContext = NullPtr;
    }

    if(platPart->d3d11Device != NullPtr)
    {
        platPart->d3d11Device->Release();
        platPart->d3d11Device = NullPtr;
    }

    return True;
}