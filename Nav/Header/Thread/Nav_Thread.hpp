#ifndef __NAV_THREAD_HPP__
#define __NAV_THREAD_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Nav_Value_Type.hpp>
#include <Thread/Posix/Nav_Posix_Thread.hpp>
#include <Thread/Win32/Nav_Win32_Thread.hpp>

class NAV_API Thread
{
    public :
    union ThreadUnion
    {
        PosixThread posixThread;
        Win32Thread win32Thread;
    };

    public :
    static Bool SupportPosix();
    static Bool SupportWin32();
};


#endif