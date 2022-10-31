#include <Thread/Posix/Nav_Posix_Thread.hpp>
#include <pthread.h>

struct PosixThread::PlatformPart
{
    pthread_t pThread;
};

PosixThread::PlatformPart* PosixThread::GetPlatformPart()
{
    return (PlatformPart*)(this + 1);
}

void PosixThread::Join()
{
    PlatformPart* platPart = this->GetPlatformPart();
    if(platPart == NullPtr){return;}

    pthread_join(platPart->pThread, NullPtr);
}

Bool PosixThread::Cancel()
{
    PlatformPart* platPart = this->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    pthread_cancel(platPart->pThread);

    return True;
}

Size PosixThread::GetSize()
{
    return sizeof(PosixThread) + sizeof(PlatformPart);
}

Bool PosixThread::Create(ThreadCall call, void* args, PosixThread* thread)
{
    if(thread == NullPtr){return False;}

    PlatformPart* platPart = thread->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    pthread_t pThread;
    pthread_create(&pThread, Null, call, args);

    platPart->pThread = pThread;

    return True;
}