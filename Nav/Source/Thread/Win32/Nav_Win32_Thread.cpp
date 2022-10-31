#include <Thread/Win32/Nav_Win32_Thread.hpp>
#include <process.h>

struct Win32Thread::PlatformPart
{
    int a;
};

Win32Thread::PlatformPart* Win32Thread::GetPlatformPart()
{
    return (PlatformPart*)(this + 1);
}

Size Win32Thread::GetSize()
{
    return sizeof(Win32Thread);
}