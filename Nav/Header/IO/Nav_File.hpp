#ifndef __NAV_FILE_HPP__
#define __NAV_FILE_HPP__

#include <Api/Nav_Api.hpp>
#include <Type/Primitive/Nav_Value_Type.hpp>

#define FILE_OPEN_MODE_READ "r"
#define FILE_OPEN_MODE_WRITE "w"
#define FILE_OPEN_MODE_READ_PLUS "r+"
#define FILE_OPEN_MODE_WRITE_PLUS "w+"
#define FILE_OPEN_MODE_READ_BINARY "rb"
#define FILE_OPEN_MODE_WRITE_BINARY "wb"

typedef char* FileOpenMode;

class NAV_API File
{
    private :
    void* _file;

    public :
    Size GetSize();
    Bool Read(void* ptr);
    Bool Write();

    public :
    static Bool Open(char* filePath, FileOpenMode openMode, File* file);
    static Bool Close(File* file);
};

#endif