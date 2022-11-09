#ifndef __NAV_LIBRARY_PLATFORM_PART_HPP__
#define __NAV_LIBRARY_PLATFORM_PART_HPP__

#include <Library/Nav_Library.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

IMPLEMENT_PLATFORM_PART(Library)
{
    HMODULE hModule;
};

#endif