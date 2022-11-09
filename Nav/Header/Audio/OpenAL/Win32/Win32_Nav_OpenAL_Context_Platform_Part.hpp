#ifndef __NAV_OPENAL_CONTEXT_PLATFORM_PART_HPP__
#define __NAV_OPENAL_CONTEXT_PLATFORM_PART_HPP__

#include <Audio/OpenAL/Nav_OpenAL_Context.hpp>
#include <al.h>
#include <alc.h>

IMPLEMENT_PLATFORM_PART(OpenALContext)
{
    ALCdevice* alcDevice;
    ALCcontext* alcContext;
};
IMPLEMENT_GET_PLATFORM_PART(OpenALContext);
IMPLEMENT_PLATFORM_PART_GET_SIZE(OpenALContext);

#endif