#include <Graphic/Type/Image/Nav_Image.hpp>
#include <Memory/Nav_Allocation.hpp>
#include <Memory/Nav_Memory_Macro.hpp>

#define QOI_IMPLEMENTATION
#include <QOI/qoi.h>

Image* Image::QOI_FromFile(char* filePath, Int32 channels)
{
    qoi_desc qoiImgDesc;
    void* qoiImgData = qoi_read(filePath, &qoiImgDesc, channels);
    if(qoiImgData == NullPtr){return NullPtr;}

    Image::Format imgFormat = (Image::Format)(qoiImgDesc.colorspace == QOI_LINEAR ? 100 + qoiImgDesc.channels : 200 + qoiImgDesc.channels);

    Size imgDataSize = sizeof(UInt8) * qoiImgDesc.channels * qoiImgDesc.width * qoiImgDesc.height;
    Image* img = (Image*)MEMORY_ALLOCATE(sizeof(Image) + imgDataSize);
    if(img == NullPtr)
    {
        MEMORY_DEALLOCATE(qoiImgData);
        return NullPtr;
    }

    MEMORY_COPY(qoiImgData, img->GetData(), imgDataSize);

    img->_format = imgFormat;
    img->_width = qoiImgDesc.width;
    img->_height = qoiImgDesc.height;
    MEMORY_DEALLOCATE(qoiImgData);

    return img;
}