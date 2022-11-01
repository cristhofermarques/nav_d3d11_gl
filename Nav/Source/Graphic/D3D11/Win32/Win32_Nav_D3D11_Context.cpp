#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Graphic/D3D11/Win32/Win32_Nav_D3D11_Context_Platform_Part.hpp>

IMPLEMENT_GET_PLATFORM_PART(D3D11Context);
IMPLEMENT_PLATFORM_PART_GET_SIZE(D3D11Context);

Bool D3D11Context::Create(D3D11Context* ctx)
{
    if(ctx == NullPtr){return False;}

    PlatformPart* platPart = ctx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT numOfDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };
    UINT featureLevelsLen = ARRAYSIZE(featureLevels);

    HRESULT res;
    for(UINT driverTypeIdx = 0U; driverTypeIdx < numOfDriverTypes; driverTypeIdx++)
    {
        D3D_DRIVER_TYPE currDriverType = driverTypes[driverTypeIdx];

        res = D3D11CreateDevice(Null, currDriverType, Null, Null, featureLevels, featureLevelsLen, D3D11_SDK_VERSION, &platPart->d3d11Device, &platPart->featureLevel, &platPart->d3d11DeviceContext);

        if(SUCCEEDED(res)){break;}
    }

    if(FAILED(res)){return False;}
    
    return True;
}

Bool D3D11Context::Destroy(D3D11Context* ctx)
{
    if(ctx == NullPtr){return False;}

    PlatformPart* platPart = ctx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    if(platPart->d3d11DeviceContext != NullPtr)
    {
        platPart->d3d11DeviceContext->Release();
    }

    if(platPart->d3d11Device != NullPtr)
    {
        platPart->d3d11Device->Release();
    }

    return True;
}