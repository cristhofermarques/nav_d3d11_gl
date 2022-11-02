#ifdef NAV_BUILD_PLATFORM_MINGW
#define NAV_API_INTERNAL_IMPORT
#endif

#include <Graphic/Nav_Graphic_Context.hpp>
#include <Graphic/Win32/Win32_Nav_Graphic_Context_Platform_Part.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Windows.h>
#include <stdio.h>

IMPLEMENT_GET_PLATFORM_PART(GraphicContext);
Size GraphicContext::GetSize()
{
    return sizeof(GraphicContext) + D3D11Context::GetSize();
}

GraphicContext::ApiType GraphicContext::GetApiType()
{
    return _apiType;
}

Bool GraphicContext::Bind(Window* wnd)
{
    if(wnd == NullPtr){return False;}

    PlatformPart* platPart = GetPlatformPart();
    if(platPart == NullPtr || platPart == NullPtr){return False;}

    Window::PlatformPart* wndPlatPart = wnd->GetPlatformPart();
    if(wndPlatPart == NullPtr){return False;}

    switch(this->GetApiType())
    {
        case GraphicContext::D3D11 :

        return ((D3D11Context*)platPart)->Bind(wndPlatPart, wnd->GetClientSize(), wnd->GetMode() != Window::WindowMode::FullScreen);

        break;
    }

    return False;
}

Bool GraphicContext::Unbind()
{
    PlatformPart* platPart = GetPlatformPart();
    if(platPart == NullPtr){return False;}

    switch(this->GetApiType())
    {
        case GraphicContext::ApiType::D3D11 :

        return ((D3D11Context*)platPart)->Unbind();

        break;
    }

    return False;
}

void GraphicContext::Clear(Float4 color)
{
    PlatformPart* platPart = GetPlatformPart();
    if(platPart == NullPtr){return;}

    switch(this->GetApiType())
    {
        case GraphicContext::ApiType::D3D11 :

        ((D3D11Context*)platPart)->Clear(color);

        break;
    }
};

void GraphicContext::Present(Bool vSync)
{
    PlatformPart* platPart = GetPlatformPart();
    if(platPart == NullPtr){return;}

    switch(this->GetApiType())
    {
        case GraphicContext::ApiType::D3D11 :

        ((D3D11Context*)platPart)->Present(vSync ? D3D11Context::On : D3D11Context::Off);

        break;
    }
};

Bool GraphicContext::Create(ApiType apiType, GraphicContext* gfxCtx)
{
    if(gfxCtx == NullPtr){return False;}
    PlatformPart* platPart = gfxCtx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    switch(apiType)
    {
        case ApiType::D3D11 :
        {
            Bool created = D3D11Context::Create((D3D11Context*)platPart);
            if(created){gfxCtx->_apiType = apiType;}
            return created;
        }
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