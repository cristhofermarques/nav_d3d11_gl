#ifdef NAV_BUILD_PLATFORM_MINGW
#define NAV_API_INTERNAL_IMPORT
#endif

#include <Graphic/Context/Nav_Graphic_Context.hpp>
#include <Graphic/Context/Win32/Win32_Nav_Graphic_Context_Platform_Part.hpp>
#include <Graphic/Api/D3D11/Context/Nav_D3D11_Context.hpp>
#include <Graphic/Api/OpenGL/Context/Nav_OpenGL_Context.hpp>
#include <Math/Nav_Comparation.hpp>
#include <Windows.h>

#define SWITCH_API_TYPE() switch(this->GetApiType())

#if NAV_BUILD_GRAPHIC_API_D3D11
    #define CASE_D3D11(code) \
        case GraphicContext::ApiType::D3D11 : \
        { \
            D3D11Context* ctx = (D3D11Context*)platPart; \
            code \
        } \
        break;

#else

    #define CASE_D3D11(code)

#endif

#if NAV_BUILD_GRAPHIC_API_OPENGL

    #define CASE_OPENGL(code) \
        case GraphicContext::ApiType::OpenGL : \
        { \
            OpenGLContext* ctx = (OpenGLContext*)platPart; \
            code \
        } \
        break;

#else

    #define CASE_OPENGL(code)

#endif

IMPLEMENT_GET_PLATFORM_PART(GraphicContext);

Size GraphicContext::GetSize()
{
    Size sizes[] =
    {
        0ULL,
        #if NAV_BUILD_GRAPHIC_API_D3D11

        D3D11Context::GetSize(),

        #endif

        #if NAV_BUILD_GRAPHIC_API_OPENGL

        OpenGLContext::GetSize(),

        #endif
    };

    return sizeof(GraphicContext) + Math::Major(sizes, ARRAYSIZE(sizes));
}

GraphicContext::ApiType GraphicContext::GetApiType()
{
    return _apiType;
}

Bool GraphicContext::Bind(Window* wnd)
{
    GET_PLATFORM_PART_OR_RETURN(False);
    GET_PLATFORM_PART(wnd, Window, False);

    SWITCH_API_TYPE()
    {
        CASE_D3D11(return ctx->Bind(wndPlatPart, wnd->GetClientSize(), wnd->GetMode() != Window::WindowMode::FullScreen););
        CASE_OPENGL(return ctx->Bind(wndPlatPart);)
    }

    return False;
}

Bool GraphicContext::Unbind()
{
    GET_PLATFORM_PART_OR_RETURN(False);
    
    SWITCH_API_TYPE()
    {
        CASE_D3D11(return ctx->Unbind();)
        CASE_OPENGL(return ctx->Unbind();)
    }

    return False;
}

void GraphicContext::Clear(Float4 color)
{
    GET_PLATFORM_PART_OR_RETURN()

    SWITCH_API_TYPE()
    {
        CASE_D3D11(ctx->Clear(color);)
        CASE_OPENGL(ctx->Clear(color);)
    }
}

void GraphicContext::Present(Bool vSync)
{
    GET_PLATFORM_PART_OR_RETURN();

    SWITCH_API_TYPE()
    {
        CASE_D3D11(ctx->Present(vSync ? D3D11Context::On : D3D11Context::Off);)
        CASE_OPENGL(ctx->Present();)
    }
}

void GraphicContext::SetViewport(Rect rect, Float minDepth, Float maxDepth)
{
    GET_PLATFORM_PART_OR_RETURN();

    SWITCH_API_TYPE()
    {
        CASE_D3D11(ctx->SetViewport(rect, minDepth, maxDepth);)
        CASE_OPENGL(ctx->SetViewport(rect, minDepth, maxDepth);)
    }
}

Bool GraphicContext::Create(ApiType apiType, GraphicContext* gfxCtx)
{
    if(gfxCtx == NullPtr){return False;}
    PlatformPart* platPart = gfxCtx->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    switch(apiType)
    {
        case ApiType::D3D11 :
        {
            if(SupportD3D11())
            {
                #if NAV_BUILD_GRAPHIC_API_D3D11
                Bool created = D3D11Context::Create((D3D11Context*)platPart);
                if(created){gfxCtx->_apiType = apiType;}
                return created;
                #endif
            }
        }
        break;

        case ApiType::OpenGL :
        {
            if(SupportOpenGL())
            {
                #if NAV_BUILD_GRAPHIC_API_OPENGL
                Bool created = OpenGLContext::Create(3, 0, (OpenGLContext*)platPart);
                if(created){gfxCtx->_apiType = apiType;}
                return created;
                #endif
            }
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
        CASE_D3D11(return D3D11Context::Destroy(ctx);)
        CASE_OPENGL(return OpenGLContext::Destroy(ctx);)
    }

    return False;
};

Bool GraphicContext::SupportD3D11()
{
    #ifdef NAV_BUILD_GRAPHIC_API_D3D11

    return True;

    #else

    return False;

    #endif
}

Bool GraphicContext::SupportOpenGL()
{
    #ifdef NAV_BUILD_GRAPHIC_API_OPENGL

    return True;

    #else

    return False;

    #endif
}