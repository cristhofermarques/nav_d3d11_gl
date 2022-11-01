#include <UI/Window/Nav_Window.hpp>
#include <Graphic/Nav_Graphic_Context.hpp>
#include <Debug/Nav_Log.hpp>
#include <stdlib.h>
#include <iostream>

int main()
{
    Window* wnd = (Window*)malloc(Window::GetSize());
    GraphicContext* ctx = (GraphicContext*)malloc(GraphicContext::GetSize());
    
    LOG_MSG(DECIMAL_LOG_FORMAT, Console::White, GraphicContext::GetSize());

    if(Window::Create("Nav", 600, 480, Window::WindowMode::Windowed, wnd))
    {
        if(GraphicContext::Create(GraphicContext::ApiType::D3D11, ctx))
        {
            while(wnd->UpdateEvents())
            {
                
            }

            GraphicContext::Destroy(ctx);
        }
        Window::Destroy(wnd);
    }

    free(ctx);
    free(wnd);
}