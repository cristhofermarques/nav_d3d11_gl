#ifndef WIN32_NAV_SOCKET_PLATFORM_PART_HPP
#define WIN32_NAV_SOCKET_PLATFORM_PART_HPP

#include <Net/Nav_Socket.hpp>
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

IMPLEMENT_PLATFORM_PART(Socket)
{
    SOCKET socket;
};

#endif