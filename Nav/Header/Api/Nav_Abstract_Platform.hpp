#ifndef __NAV_ABSTRACT_PLATFORM_HPP__
#define __NAV_ABSTRACT_PLATFORM_HPP__

#include "Nav_Type.hpp"

#define DECLARE_PLATFORM_PART() struct PlatformPart
#define IMPLEMENT_PLATFORM_PART(type) struct type::PlatformPart

#define DECLARE_PLATFORM_PART_UNION() union PlatformPart
#define IMPLEMENT_PLATFORM_PART_UNION(type) union type::PlatformPart

#define DECLARE_PLATFORM_PART_GET_SIZE() static Size GetSize()
#define IMPLEMENT_PLATFORM_PART_GET_SIZE(type) Size type::GetSize(){return sizeof(type) + sizeof(type::PlatformPart);}

#define DECLARE_GET_PLATFORM_PART() PlatformPart* GetPlatformPart()
#define IMPLEMENT_GET_PLATFORM_PART(type) type::PlatformPart* type::GetPlatformPart(){return (type::PlatformPart*)(this + 1);}

#define GET_PLATFORM_PART_OR_RETURN(ret) \
    PlatformPart* platPart = this->GetPlatformPart(); \
    if(platPart == NullPtr){return ret;}

#define GET_PLATFORM_PART(obj, type, ret) \
    if(obj == NullPtr){return ret;} \
    type::PlatformPart* obj##PlatPart = obj->GetPlatformPart(); \
    if(obj##PlatPart == NullPtr){return ret;}

#endif