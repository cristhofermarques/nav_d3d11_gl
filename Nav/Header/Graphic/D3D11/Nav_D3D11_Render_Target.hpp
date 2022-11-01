#ifndef __NAV_D3D11_RENDER_TARGET_HPP__
#define __NAV_D3D11_RENDER_TARGET_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <UI/Window/Nav_Window.hpp>

class NAV_API D3D11RenderTarget
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    public :
    Bool BindWindow(Window::PlatformPart* wndPlatPart, Int2 wndSize, Bool wndIsWindowed, D3D11Context::PlatformPart* d3d11CtxPlatPart, UInt32 refreshRateNumerator = 60U, UInt32 refreshRateDenominator = 1U, UInt32 sampleCount = 1U, UInt32 sampleQuality = 0U, UInt32 bufferCount = 2U);
    Bool BindContext(D3D11Context::PlatformPart* d3d11CtxPlatPart);
    void Present();
    void Clear();

    public :
    static Bool Create(D3D11Context::PlatformPart* ctxPlatPart, D3D11RenderTarget* renderTarget);
    static Bool Destroy(D3D11RenderTarget* renderTarget);
};

#endif