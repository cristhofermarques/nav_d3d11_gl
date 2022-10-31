#include <Graphic/D3D11/Nav_D3D11_Context.hpp>
#include <Nav_Log.hpp>
#include <stdlib.h>
#include <iostream>

int main()
{
    D3D11Context* ctx = (D3D11Context*)malloc(D3D11Context::GetSize());
    LOG_MSG(HEXADECIMAL_LOG_FORMAT, Console::PrintColor::Gray, ctx);

    if(D3D11Context::Create(ctx))
    {
        std::cout << "created";

        if(D3D11Context::Destroy(ctx))
        {
            std::cout << "removed";
        }
    }
}