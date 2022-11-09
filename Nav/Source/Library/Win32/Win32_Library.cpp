#include <Library/Nav_Library.hpp>
#include <Library/Win32/Win32_Nav_Library_Platform_Part.hpp>

IMPLEMENT_GET_PLATFORM_PART(Library);
IMPLEMENT_PLATFORM_PART_GET_SIZE(Library);

void* Library::GetFunctionAddress(char* functionName)
{
    if(functionName == NullPtr){return NullPtr;}
    GET_PLATFORM_PART_OR_RETURN(NullPtr);

    FARPROC proc = GetProcAddress(platPart->hModule, (LPCSTR)functionName);
    return (void*)proc;
}   

Bool Library::Load(char* libPath, Library* lib)
{
    if(libPath == NullPtr){return False;}
    GET_PLATFORM_PART(lib, Library, False);

    HMODULE hModule = LoadLibrary((LPCSTR)libPath);
    if(hModule == NullPtr){return False;}

    libPlatPart->hModule = hModule;
    return True;
}

Bool Library::GetLoaded(char* libName, Library* lib)
{
    if(libName == NullPtr){return False;}
    GET_PLATFORM_PART(lib, Library, False);

    HMODULE hModule = GetModuleHandleA((LPCSTR)libName);
    if(hModule == NullPtr){return False;}

    libPlatPart->hModule = hModule;
    return True;
}

Bool Library::Unload(Library* lib)
{
    GET_PLATFORM_PART(lib, Library, False);

    if(FreeLibrary(libPlatPart->hModule)){return False;}

    libPlatPart->hModule = 0;
    return True;
}