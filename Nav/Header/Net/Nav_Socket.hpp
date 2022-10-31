#ifndef __NAV_SOCKET_HPP__
#define __NAV_SOCKET_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>

class NAV_API Socket
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();
    DECLARE_GET_PLATFORM_PART();

    public :
    static Bool Create(Socket* outSocket);
    static Bool Destroy(Socket* outSocket);
};

#endif