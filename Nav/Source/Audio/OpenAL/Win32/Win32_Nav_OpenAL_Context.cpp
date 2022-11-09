#include <Audio/OpenAL/Nav_OpenAL_Context.hpp>
#include <Audio/OpenAL/Win32/Win32_Nav_OpenAL_Context_Platform_Part.hpp>
#include <al.h>
#include <alc.h>

Bool OpenALContext::Create(OpenALContext* alCtx)
{
    if(alCtx == NullPtr){return False;}

    OpenALContext::PlatformPart* platPart = alCtx->GetPlatformPart();
    if(alCtx == NullPtr){return False;}

    ALCdevice* alcDevice = NullPtr;
    alcDevice = alcOpenDevice(Null);
    if(!alcDevice){return False;}

    ALCcontext* alcContext = NullPtr;
    alcContext = alcCreateContext(alcDevice, Null);
    if(!alcContext)
    {
        alcCloseDevice(alcDevice);
        return False;
    }

    alcMakeContextCurrent(alcContext);

    platPart->alcDevice = alcDevice;
    platPart->alcContext = alcContext;
    return True;
}

Bool OpenALContext::Destroy(OpenALContext* alCtx)
{
    if(alCtx == NullPtr){return False;}

    OpenALContext::PlatformPart* platPart = alCtx->GetPlatformPart();
    if(alCtx == NullPtr){return False;}

    if(platPart->alcContext)
    {
        alcMakeContextCurrent(Null);
        alcDestroyContext(platPart->alcContext);

        if(platPart->alcDevice)
        {
            alcCloseDevice(platPart->alcDevice);
        }
    }

    return True;
}