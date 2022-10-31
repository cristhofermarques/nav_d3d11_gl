#include <Graphic/Nav_Graphic_Context.hpp>
#include <Graphic/Win32/Win32_Nav_Graphic_Context_Platform_Part.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Windows.h>

GraphicContext::ApiType GraphicContext::GetApiType()
{
    return ApiType::D3D11;
}
