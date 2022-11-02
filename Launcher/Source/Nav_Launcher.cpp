#include <UI/Window/Nav_Window.hpp>
#include <Graphic/Nav_Graphic_Context.hpp>
#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Debug/Nav_Log.hpp>
#include <stdlib.h>
#include <iostream>

int main()
{
    Window* wnd = (Window*)malloc(Window::GetSize());
    //D3D11Context* ctx = (D3D11Context*)malloc(D3D11Context::GetSize());
    GraphicContext* ctx = (GraphicContext*)malloc(GraphicContext::GetSize());

    //LOG_MSG(DECIMAL_LOG_FORMAT DECIMAL_LOG_FORMAT, Console::Blue, gfxCtx, gfxCtx->GetPlatformPart());
    
    if(Window::Create("Nav", 600, 480, Window::WindowMode::Windowed, wnd))
    {
        if(GraphicContext::Create(GraphicContext::ApiType::D3D11, ctx))
        {
            //if(ctx->Bind(wnd->GetPlatformPart(), wnd->GetClientSize(), wnd->GetMode() != Window::FullScreen))
            if(ctx->Bind(wnd))
            {
                while(wnd->UpdateEvents())
                {
                    ctx->Clear(FLOAT4(1, 0, 0, 1));
                    ctx->Present(D3D11Context::VSyncMode::Off);
                }

                ctx->Unbind();
            }

            GraphicContext::Destroy(ctx);
            //D3D11Context::Destroy(ctx);
        }
        Window::Destroy(wnd);
    }

    free(ctx);
    free(wnd);
}