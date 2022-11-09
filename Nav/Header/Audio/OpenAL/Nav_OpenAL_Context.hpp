#ifndef __NAV_OPENAL_CONTEXT_HPP__
#define __NAV_OPENAL_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>

class OpenALContext
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

    public :
    static Bool Create(OpenALContext* alCtx);
    static Bool Destroy(OpenALContext* alCtx);
};

#endif