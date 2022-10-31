#include <Graphic/Nav_Graphic_Context.h>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Windows.h>

union GraphicContext::PlatformUnion
{
    D3D11Context d3d11Context;
};

GraphicContext::PlatformUnion* GraphicContext::GetPlatformUnion()
{
    return (PlatformUnion*)(this + 1);
}

GraphicContext::ApiType GraphicContext::GetApiType()
{
    return ApiType::D3D11;
}

Size GraphicContext::GetSize()
{
    return sizeof(GraphicContext) + sizeof(PlatformUnion);
}