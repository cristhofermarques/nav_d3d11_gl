#include <UI/Window/Nav_Window.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Graphic/D3D11/Nav_D3D11_Render_Target.hpp>
#include <Graphic/D3D11/Win32/Win32_Nav_D3D11_Context_Platform_Part.hpp>
#include <Debug/Nav_Log.hpp>
#include <stdlib.h>
#include <iostream>

int main()
{
    Window* wnd = (Window*)malloc(Window::GetSize());
    D3D11Context* ctx = (D3D11Context*)malloc(D3D11Context::GetSize());
    D3D11RenderTarget* renderTarget = (D3D11RenderTarget*)malloc(D3D11RenderTarget::GetSize());

    if(Window::Create("Nav", 600, 480, Window::WindowMode::Windowed, wnd))
    {
        Int2 size = wnd->GetClientSize();
        std::cout << size.x << " " << size.y << '\n';

        if(D3D11Context::Create(ctx))
        {
            if(D3D11RenderTarget::Create(ctx->GetPlatformPart(), renderTarget))
            {
                if(renderTarget->BindWindow(wnd->GetPlatformPart(), wnd->GetClientSize(), wnd->GetMode() != Window::FullScreen, ctx->GetPlatformPart()))
                {
                    if(renderTarget->BindContext(ctx->GetPlatformPart()))
                    {
                        Int2 size = wnd->GetClientSize();

                        D3D11_VIEWPORT viewPort = NullStruct;
                        viewPort.TopLeftX = 0;
                        viewPort.TopLeftY = 0;
                        viewPort.Width = size.x;
                        viewPort.Height = size.y;
                        viewPort.MinDepth = 0.0f;
                        viewPort.MaxDepth = 1.0f;

                        ctx->GetPlatformPart()->d3d11DeviceContext->RSSetViewports(1, &viewPort);

                        while(wnd->UpdateEvents())
                        {

                        }
                    }
                }

                D3D11RenderTarget::Destroy(renderTarget);
            }
            D3D11Context::Destroy(ctx);
        }

        Window::Destroy(wnd);
    }

    free(renderTarget);
    free(ctx);
    free(wnd);
}