#include <Thread/Nav_Thread.hpp>

Bool Thread::SupportPosix()
{

#ifdef NAV_BUILD_THREAD_POSIX

    return True;

#else

    return False;

#endif

}

Bool Thread::SupportWin32()
{
#ifdef NAV_BUILD_THREAD_WIN32

    return True;

#else

    return False;

#endif

}