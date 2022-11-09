#include <IO/Nav_File.hpp>
#include <stdio.h>

Size File::GetSize()
{
    if(this->_file == NullPtr){return 0ULL;}
    
    FILE* filePtr = (FILE*)this->_file;
    if(fseek(filePtr, 0, SEEK_END)){return 0ULL;}

    UInt32 size = ftell(filePtr);
    
    fseek(filePtr, 0, SEEK_SET);

    return size;
}

Bool File::Read(void* ptr)
{
    if(this->_file == NullPtr || ptr == NullPtr){return False;}

    FILE* filePtr = (FILE*)this->_file;
    if(filePtr == NullPtr){return False;}

    Size size = GetSize();
    if(size == 0){return False;}

    return fread(ptr, size, 1, filePtr) > 0 ? True : False;
}

Bool File::Open(char* filePath, FileOpenMode openMode, File* file)
{
    if(file == NullPtr || filePath == NullPtr || openMode == Null){return False;}

    FILE* hFile = fopen(filePath, openMode);
    if(hFile == NullPtr){return False;}

    file->_file = hFile;

    return True;
}

Bool File::Close(File* file)
{
    if(file == NullPtr || file->_file == Null){return False;}
    FILE* filePtr = (FILE*)file->_file;
    return fclose(filePtr) == 0 ? True : False;
}