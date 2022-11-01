#include <Graphic/Nav_Graphic_Context.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Windows.h>

IMPLEMENT_PLATFORM_PART_UNION(GraphicContext)
{
    D3D11Context* d3d11Context;
};
IMPLEMENT_GET_PLATFORM_PART(GraphicContext);
Size GraphicContext::GetSize()
{
    return sizeof(GraphicContext) + D3D11Context::GetSize();
}

GraphicContext::ApiType GraphicContext::GetApiType()
{
    return _apiType;
}

Bool GraphicContext::Create(ApiType apiType, GraphicContext* gfxCtx)
{
    if(gfxCtx == NullPtr){return False;}
    PlatformPart* platPart = gfxCtx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    switch(apiType)
    {
        case ApiType::D3D11 :
        return D3D11Context::Create((D3D11Context*)platPart);
        break;
    }

    return False;
};

Bool GraphicContext::Destroy(GraphicContext* gfxCtx)
{
    if(gfxCtx == NullPtr){return False;}
    PlatformPart* platPart = gfxCtx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    switch(gfxCtx->GetApiType())
    {
        case ApiType::D3D11 :
        return D3D11Context::Destroy((D3D11Context*)platPart);
        break;
    }

    return False;
};

Bool GraphicContext::SupportD3D11()
{
    return True;
}