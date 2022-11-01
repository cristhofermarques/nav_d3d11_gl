#ifndef __NAV_POSIX_THREAD_HPP__
#define __NAV_POSIX_THREAD_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>
#include <Type/Nav_Value_Type.hpp>
#include <Type/Nav_Function_Pointer.hpp>

class NAV_API PosixThread
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();
    typedef FUNCTION_POINTER_DEFINE(void*, ThreadCall, void*);

    public :
    void Join();
    Bool Cancel();

    public :
    static Bool Create(ThreadCall call, void* args, PosixThread* thread);
};

#endif