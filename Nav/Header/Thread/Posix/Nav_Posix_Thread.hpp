#ifndef __NAV_POSIX_THREAD_HPP__
#define __NAV_POSIX_THREAD_HPP__

#include <Api/Nav_Api.h>
#include <Type/Nav_Value_Type.h>
#include <Type/Nav_Function_Pointer.hpp>

class NAV_API PosixThread
{
    public :
    struct PlatformPart;
    typedef FUNCTION_POINTER_DEFINE(void*, ThreadCall, void*);

    public :
    PlatformPart* GetPlatformPart();
    void Join();
    Bool Cancel();

    public :
    static Size GetSize();
    static Bool Create(ThreadCall call, void* args, PosixThread* thread);
};

#endif