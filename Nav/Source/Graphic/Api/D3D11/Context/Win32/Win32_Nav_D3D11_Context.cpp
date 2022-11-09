#include <Graphic/Api/D3D11/Context/Nav_D3D11_Context.hpp>
#include <Graphic/Api/D3D11/Context/Win32/Win32_Nav_D3D11_Context_Platform_Part.hpp>
#include <UI/Window/Win32/Win32_Nav_Window_Platform_Part.hpp>
#include <Graphic/Resource/Nav_Mesh.hpp>
#include <Graphic/Type/Nav_Vertex_Layout.hpp>
#include <Memory/Nav_Allocation.hpp>
#include <Debug/Nav_Log.hpp>
#include <d3dcompiler.h>
#include <iostream>

IMPLEMENT_GET_PLATFORM_PART(D3D11Context);
IMPLEMENT_PLATFORM_PART_GET_SIZE(D3D11Context);

DXGI_FORMAT GetFormatByElement(UInt8 element);

Bool D3D11Context::HasD3DDevice()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->d3d11Device == NullPtr) ? False : True;
}
Bool D3D11Context::HasD3DContext()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->d3d11DeviceContext == NullPtr) ? False : True;
}
Bool D3D11Context::HasDevice()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->dxgiDevice == NullPtr) ? False : True;
}
Bool D3D11Context::HasAdapter()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->dxgiAdapter == NullPtr) ? False : True;
}
Bool D3D11Context::HasFactory()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->dxgiFactory == NullPtr) ? False : True;
}
Bool D3D11Context::HasSwapChain()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->swapChain == NullPtr) ? False : True;
}
Bool D3D11Context::HasRenderTargetView()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    return (platPart->renderTargetView == NullPtr) ? False : True;
}

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

Void D3D11Context::Clear(Float4 color)
{
    if(_isBinded)
    {
        PlatformPart* platPart = GetPlatformPart();
        platPart->d3d11DeviceContext->ClearRenderTargetView(platPart->renderTargetView, &color.x);
    }
}
Void D3D11Context::Present(VSyncMode vSyncMode)
{
    if(_isBinded)
    {
        PlatformPart* platPart = GetPlatformPart();
        platPart->swapChain->Present((UINT)vSyncMode, Null);
        platPart->d3d11DeviceContext->OMSetRenderTargets(1, &platPart->renderTargetView, NullPtr);
    }
}
Void D3D11Context::SetViewport(Rect rect, Float minDepth, Float maxDepth)
{
    if(_isBinded)
    {
        Int2 rectSize = rect.GetSize();

        D3D11_VIEWPORT viewportRect = NullStruct;
        viewportRect.TopLeftX = (FLOAT)rect.left;
        viewportRect.TopLeftY = (FLOAT)rect.top;
        viewportRect.Width = (FLOAT)rectSize.x;
        viewportRect.Height = (FLOAT)rectSize.y;
        viewportRect.MinDepth = minDepth;
        viewportRect.MaxDepth = maxDepth;

        PlatformPart* platPart = this->GetPlatformPart();
        platPart->d3d11DeviceContext->RSSetViewports(1, &viewportRect);
    }
}

Bool D3D11Context::CreateMesh(Mesh::Layout meshLayout, void* vertexBuffer, Size vertexBufferSize, Mesh::Triangle* triangleBuffer, Size triangleCount, D3D11Mesh* mesh, D3D11_USAGE vertexBufferUsage, UInt32 vertexBufferCpuAccessFlags, UInt32 vertexBufferMiscFlags, D3D11_USAGE indexBufferUsage, UInt32 indexBufferCpuAccessFlags, UInt32 indexBufferMiscFlags)
{
    if(mesh == NullPtr || vertexBuffer == NullPtr || triangleBuffer == NullPtr){return False;}
    GET_PLATFORM_PART_OR_RETURN(False);
    if(platPart->d3d11Device == NullPtr){return False;}

    D3D11_BUFFER_DESC vertexBuffDesc = NullStruct;
    vertexBuffDesc.Usage = vertexBufferUsage;
    vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBuffDesc.StructureByteStride = meshLayout.GetVertexSize();
    vertexBuffDesc.ByteWidth = vertexBufferSize;
    vertexBuffDesc.CPUAccessFlags = vertexBufferCpuAccessFlags;
    vertexBuffDesc.MiscFlags = vertexBufferMiscFlags;

    D3D11_SUBRESOURCE_DATA vertexSubResData = NullStruct;
    vertexSubResData.pSysMem = vertexBuffer;
    vertexSubResData.SysMemPitch = 0U;
    vertexSubResData.SysMemSlicePitch = 0U;

    ID3D11Buffer* vertexBuff = NullPtr;
    if(FAILED(platPart->d3d11Device->CreateBuffer(&vertexBuffDesc, &vertexSubResData, &vertexBuff))){return False;}

    D3D11_BUFFER_DESC indexBuffDesc = NullStruct;
    indexBuffDesc.Usage = indexBufferUsage;
    indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBuffDesc.StructureByteStride = sizeof(UInt16);
    indexBuffDesc.ByteWidth = sizeof(Mesh::Triangle) * triangleCount;
    indexBuffDesc.CPUAccessFlags = indexBufferCpuAccessFlags;
    indexBuffDesc.MiscFlags = indexBufferMiscFlags;

    D3D11_SUBRESOURCE_DATA indexSubResData = NullStruct;
    indexSubResData.pSysMem = triangleBuffer;
    indexSubResData.SysMemPitch = 0U;
    indexSubResData.SysMemSlicePitch = 0U;
    

    ID3D11Buffer* indexBuff = NullPtr;
    if(FAILED(platPart->d3d11Device->CreateBuffer(&indexBuffDesc, &indexSubResData, &indexBuff)))
    {
        vertexBuff->Release();
        return False;
    }

    mesh->info.layout = meshLayout;
    mesh->info.vertexBufferSize = vertexBufferSize;
    mesh->info.triangleCount = triangleCount;
    mesh->vertexBuffer = vertexBuff;
    mesh->indexBuffer = indexBuff;

    return True;
}
Bool D3D11Context::CreateShader(D3D11Shader::Descriptor shaderDesc, D3D11Shader* shader, D3D11_INPUT_CLASSIFICATION inputClass)
{
    if(shader == NullPtr || shaderDesc.layout.GetCount() == 0ULL){return False;}
    GET_PLATFORM_PART_OR_RETURN(False);

    if(shaderDesc.vertexShaderPath == NullPtr){return False;}
    ID3DBlob* blob = NullPtr;
    if(FAILED(D3DReadFileToBlob(shaderDesc.vertexShaderPath, &blob)))
    {
        LOG_ERROR("red vertex shader byte code to blob");
        return False;
    }
    
    ID3D11VertexShader* vertexShader = NullPtr;
    if FAILED(platPart->d3d11Device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NullPtr, &vertexShader))
    {
        LOG_ERROR("create vertex shader");
        blob->Release();
        return False;
    }


    D3D11_INPUT_ELEMENT_DESC* inElements = (D3D11_INPUT_ELEMENT_DESC*)MEMORY_ALLOCATE(sizeof(D3D11_INPUT_ELEMENT_DESC) * shaderDesc.layout.GetCount());
    memset(inElements, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * shaderDesc.layout.GetCount());
    UINT offset = 0U;

    for(Size inElementIdx = 0ULL; inElementIdx < shaderDesc.layout.GetCount(); inElementIdx++)
    {
        Shader::Layout::Element element = * shaderDesc.layout.GetIndex(inElementIdx);
        DXGI_FORMAT format = GetFormatByElement(element.type);

        inElements[inElementIdx].SemanticName = shaderDesc.layout.GetIndex(inElementIdx)->name.data;
        inElements[inElementIdx].SemanticIndex = 0;
        inElements[inElementIdx].Format = format;
        inElements[inElementIdx].InputSlot = 0;
        inElements[inElementIdx].AlignedByteOffset = offset;
        inElements[inElementIdx].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inElements[inElementIdx].InstanceDataStepRate = 0;

        offset += (UINT)shaderDesc.layout.GetIndex(inElementIdx)->type;
    }

    ID3D11InputLayout* inputLayout = NullPtr;
    if(FAILED(platPart->d3d11Device->CreateInputLayout(inElements, shaderDesc.layout.GetCount(), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout)))
    {
        LOG_ERROR("create input layout");
        MEMORY_DEALLOCATE(inElements);
        vertexShader->Release();
        blob->Release();
        return False;
    }

    MEMORY_DEALLOCATE(inElements);
    blob->Release();

    if(shaderDesc.pixelShaderPath == NullPtr)
    {
        LOG_ERROR("pixel shader path not specified");
        vertexShader->Release();
        return False;
    }

    if(FAILED(D3DReadFileToBlob((LPCWSTR)shaderDesc.pixelShaderPath, &blob)))
    {
        LOG_ERROR("red pixel shader byte code to blob");
        vertexShader->Release();
        return False;
    }

    ID3D11PixelShader* pixelShader = NullPtr;
    if FAILED(platPart->d3d11Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &pixelShader))
    {
        LOG_ERROR("create pixel shader");
        vertexShader->Release();
        blob->Release();
        return False;
    }

    shader->inputLayout = inputLayout;
    shader->vertexShader = vertexShader;
    shader->pixelShader = pixelShader;
    shader->layout = shaderDesc.layout;
    shader->layout.SetLockedState(True);

    return True;
}
Bool D3D11Context::CreateTexture2D(Image* image, D3D11Texture2D* texture2d, UInt32 mipLevels, UInt32 arraySize, DXGI_SAMPLE_DESC sampleDesc, D3D11_USAGE texture2dUsage, UInt32 bindFlags, D3D11_FILTER sampleFilter, D3D11_TEXTURE_ADDRESS_MODE addressMode, Float mipLodBias, UInt32 maxAnisotropy, D3D11_COMPARISON_FUNC comparationFunc, Color borderColor)
{
    if(!_isBinded || texture2d == NullPtr || image == NullPtr){return False;}
    GET_PLATFORM_PART_OR_RETURN(False);

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = image->GetWidth();
    texture2dDesc.Height = image->GetHeight();
    texture2dDesc.MipLevels = mipLevels;
    texture2dDesc.ArraySize = arraySize;
    texture2dDesc.Format = image->IsLinear() ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    texture2dDesc.SampleDesc = sampleDesc;
    texture2dDesc.Usage = texture2dUsage;
    texture2dDesc.BindFlags = (UINT)bindFlags;

    D3D11_SUBRESOURCE_DATA texture2dSubResData = {};
    texture2dSubResData.pSysMem = image->GetData();
    texture2dSubResData.SysMemPitch = image->GetWidth() * image->GetColorSize();

    LOG(DECIMAL_LOG_FORMAT, Console::Blue, image->GetColorSize());

    ID3D11Texture2D* d3d11Texture2d = NullPtr;
    if FAILED(platPart->d3d11Device->CreateTexture2D(&texture2dDesc, &texture2dSubResData, &d3d11Texture2d))
    {
        LOG_ERROR("create texture 2d");
        return False;
    }

    ID3D11ShaderResourceView* shaderResView = NullPtr;
    if FAILED(platPart->d3d11Device->CreateShaderResourceView(d3d11Texture2d, NullPtr, &shaderResView))
    {
        LOG_ERROR("create shader resource view");
        d3d11Texture2d->Release();
        return False;
    }

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = sampleFilter;
    samplerDesc.AddressU = addressMode;
    samplerDesc.AddressV = addressMode;
    samplerDesc.AddressW = addressMode;
    samplerDesc.MipLODBias = mipLodBias;
    samplerDesc.MaxAnisotropy = maxAnisotropy;
    samplerDesc.ComparisonFunc = comparationFunc;
    Float4 uNormColor = borderColor.GetUNorm();
    samplerDesc.BorderColor[0] = uNormColor.x;
    samplerDesc.BorderColor[1] = uNormColor.y;
    samplerDesc.BorderColor[2] = uNormColor.z;
    samplerDesc.BorderColor[3] = uNormColor.w;
    samplerDesc.MinLOD = FLT_MIN;
    samplerDesc.MaxLOD = FLT_MAX;

    ID3D11SamplerState* samplerState = NullPtr;
    if FAILED(platPart->d3d11Device->CreateSamplerState(&samplerDesc, &samplerState))
    {
        LOG_ERROR("create sampler state");
        shaderResView->Release();
        d3d11Texture2d->Release();
        return False;
    }

    texture2d->d3d11Texture2d = d3d11Texture2d;
    texture2d->d3d11ShaderResourceView = shaderResView;
    texture2d->d3d11SamplerState = samplerState;

    return True;
}

Bool D3D11Context::BindMesh(D3D11Mesh* mesh)
{
    GET_PLATFORM_PART_OR_RETURN(False);

    UINT stride = mesh->info.layout.GetVertexSize();
    UINT offset = 0U;
    platPart->d3d11DeviceContext->IASetVertexBuffers(0U, 1U, & mesh->vertexBuffer, &stride, &offset);
    platPart->d3d11DeviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R16_UINT, 0U);

    return True;
};
Bool D3D11Context::BindShader(D3D11Shader* shader)
{
    GET_PLATFORM_PART_OR_RETURN(False);

    platPart->d3d11DeviceContext->IASetInputLayout(shader->inputLayout);
    platPart->d3d11DeviceContext->VSSetShader(shader->vertexShader, NullPtr, 0);
    platPart->d3d11DeviceContext->PSSetShader(shader->pixelShader, NullPtr, 0);

    return True;
};
Bool D3D11Context::BindTexture2D(D3D11Texture2D* texture2d)
{
    if(texture2d == NullPtr){return False;}
    GET_PLATFORM_PART_OR_RETURN(False);

    platPart->d3d11DeviceContext->PSSetShaderResources(0U, 1U, &texture2d->d3d11ShaderResourceView);
    platPart->d3d11DeviceContext->PSSetSamplers(0U, 1U, &texture2d->d3d11SamplerState);

    return True;
}

Void D3D11Context::Draw(UInt32 idxCount, UInt32 startIdx, Int32 baseVtxLoc)
{
    GET_PLATFORM_PART_OR_RETURN();
    platPart->d3d11DeviceContext->DrawIndexed(idxCount, startIdx, baseVtxLoc);
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

    #ifdef NAV_BUILD_MODE_DEBUG

    UINT flags = D3D11_CREATE_DEVICE_DEBUG;

    #else

    UINT flags = 0U;

    #endif

    if FAILED(D3D11CreateDevice(Null, D3D_DRIVER_TYPE_HARDWARE, Null, flags, featureLevels, featureLevelsLen, D3D11_SDK_VERSION, &platPart->d3d11Device, &platPart->featureLevel, &platPart->d3d11DeviceContext)){return False;}

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

DXGI_FORMAT GetFormatByElement(UInt8 element)
{
    switch(element)
    {
        case VEC2_ELEMENT :
            return DXGI_FORMAT_R32G32_FLOAT;
        break;

        case VEC3_ELEMENT :
            return DXGI_FORMAT_R32G32B32_FLOAT;
        break;

        case VEC4_ELEMENT :
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
    }

    return (DXGI_FORMAT)Null;
}