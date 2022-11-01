#include <Net/Nav_Socket.hpp>
#include <Net/Win32/Win32_Nav_Socket_Platform_Part.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>

Bool initialized = False;
WSADATA wsaData = NullStruct;
UInt32 socketCount = 0U;

IMPLEMENT_PLATFORM_PART_GET_SIZE(Socket);
IMPLEMENT_GET_PLATFORM_PART(Socket);

Bool Init()
{
    if(initialized){return True;}
    
    initialized = (Bool)(WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

    return initialized;
}

Bool End()
{
    if(!initialized){return False;}

    initialized = (WSACleanup() == 0) ? False : True;

    return !initialized;
}

Bool Socket::Create(Socket* outSocket)
{
    if(outSocket == NullPtr){return False;}
    if(!Init()){return False;}

    PlatformPart* platPart = outSocket->GetPlatformPart();
    if(platPart == NullPtr){return False;}

    SOCKET sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sck == SOCKET_ERROR){return False;}

    platPart->socket = sck;
    socketCount++;
    return True;
}

Bool Socket::Destroy(Socket* outSocket)
{
    if(outSocket == NullPtr){return False;}

    PlatformPart* platPart = outSocket->GetPlatformPart();
    if(platPart == NullPtr || platPart->socket == SOCKET_ERROR){return False;}
    closesocket(platPart->socket);
    socketCount--;
    
    if(socketCount == 0){End();}

    return True;
}