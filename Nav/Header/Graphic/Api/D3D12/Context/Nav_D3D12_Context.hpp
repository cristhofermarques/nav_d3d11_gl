#ifndef __NAV_D3D12_CONTEXT_HPP__
#define __NAV_D3D12_CONTEXT_HPP__

#include <Api/Nav_Api.hpp>
#include <Api/Nav_Abstract_Platform.hpp>

class NAV_API D3D12Context
{
    public :
    DECLARE_PLATFORM_PART();
    DECLARE_GET_PLATFORM_PART();
    DECLARE_PLATFORM_PART_GET_SIZE();

};

#endif