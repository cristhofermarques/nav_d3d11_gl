#ifndef __NAV_D3D11_CONTEXT_H__
#define __NAV_D3D11_CONTEXT_H__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Nav_Value_Type.hpp>
#include <Math/Nav_Float4.hpp>
#include <UI/Window/Nav_Window.hpp>

class NAV_API D3D11Context
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    enum VSyncMode
    {
        Off = 0U,
        Half = 2U,
        On = 1U
    };

    private :
    Bool _isBinded;

    public :
    Bool Bind(Window::PlatformPart* wndPlatPart, Int2 wndSize, Bool wndIsWindowed, UInt32 refreshRateNumerator = 60U, UInt32 refreshRateDenominator = 1U, UInt32 sampleCount = 1U, UInt32 sampleQuality = 0U, UInt32 bufferCount = 2U);
    Bool Unbind();

    public :
    void Clear(Float4 color);
    void Present(VSyncMode vSyncMode);

    public :
    static Bool Create(D3D11Context* ctx);
    static Bool Destroy(D3D11Context* ctx);
};

#endif