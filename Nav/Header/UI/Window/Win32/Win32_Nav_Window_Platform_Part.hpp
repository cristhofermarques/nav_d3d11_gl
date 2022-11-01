#ifndef __WIN32_NAV_WINDOW_PLATFORM_PART_HPP__
#define __WIN32_NAV_WINDOW_PLATFORM_PART_HPP__

#include <UI/Window/Nav_Window.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

IMPLEMENT_PLATFORM_PART(Window)
{
    HWND hWnd;
    HDC hDc;
};

#endif