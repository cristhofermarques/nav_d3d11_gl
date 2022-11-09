#ifndef __NAV_IMAGE_HPP__
#define __NAV_IMAGE_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Aka.hpp>

#define IMAGE_FORMAT_LINEAR_R    ((UInt8)101)
#define IMAGE_FORMAT_LINEAR_RG   ((UInt8)102)
#define IMAGE_FORMAT_LINEAR_RGB  ((UInt8)103)
#define IMAGE_FORMAT_LINEAR_RGBA ((UInt8)104)
#define IMAGE_FORMAT_SRGB_R      ((UInt8)201)
#define IMAGE_FORMAT_SRGB_RG     ((UInt8)202)
#define IMAGE_FORMAT_SRGB_RGB    ((UInt8)203)
#define IMAGE_FORMAT_SRGB_RGBA   ((UInt8)204)

class NAV_API Image
{
    public :
    typedef UInt8 Format;

    private :
    Format _format;
    UInt32 _width;
    UInt32 _height;

    public :
    inline Format GetFormat()
    {
        return _format;
    }
    inline UInt32 GetWidth()
    {
        return _width;
    }
    inline UInt32 GetHeight()
    {
        return _height;
    }
    inline Void* GetData()
    {
        return (void*)(this + 1);
    }
    inline Bool IsLinear()
    {
        return this->_format < 200;
    }
    inline Bool IsSrgb()
    {
        return this->_format > 200;
    }
    inline Size GetColorSize()
    {
        return (IsLinear() ? _format - 100 : _format - 200) * sizeof(UInt8);
    }

    public :
    static Image* QOI_FromFile(char* filePath, Int32 channels = 0);
};

#endif