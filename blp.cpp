#include "blp.h"
#include <string.h>


// Forward declaration of "internal" functions
tBGRAPixel* blp_convert_paletted_no_alpha(uint8_t* pSrc, tBLP2Header* pHeader, unsigned int width, unsigned int height);
tBGRAPixel* blp_convert_paletted_alpha1(uint8_t* pSrc, tBLP2Header* pHeader, unsigned int width, unsigned int height);
tBGRAPixel* blp_convert_paletted_alpha8(uint8_t* pSrc, tBLP2Header* pHeader, unsigned int width, unsigned int height);


tBLP2Header* blp_processFile(FILE* pFile)
{
    tBLP2Header* pHeader = new tBLP2Header();
    
    fseek(pFile, 0, SEEK_SET);
    fread((void*) pHeader, sizeof(uint8_t), 4, pFile);

    if (strncmp((char*) pHeader->magic, "BLP2", 4) != 0)
    {
        delete pHeader;
        return 0;
    }

    fread((void*) &pHeader->type, sizeof(tBLP2Header) - 4 * sizeof(uint8_t), 1, pFile);
    
    pHeader->nbMipLevels = 0;
    while ((pHeader->offsets[pHeader->nbMipLevels] != 0) && (pHeader->nbMipLevels < 16))
        ++pHeader->nbMipLevels;
    
    return pHeader;
}


tBLPFormat blp_format(tBLP2Header* pHeader)
{
    if (pHeader->encoding == BLP_ENCODING_UNCOMPRESSED)
        return tBLPFormat((pHeader->encoding << 16) | (pHeader->alphaDepth << 8));
    
    return tBLPFormat((pHeader->encoding << 16) | (pHeader->alphaDepth << 8) | pHeader->alphaEncoding);
}


unsigned int blp_width(tBLP2Header* pHeader, unsigned int mipLevel)
{
    // Check the mip level
    if (mipLevel >= pHeader->nbMipLevels)
        mipLevel = pHeader->nbMipLevels - 1;

    return (pHeader->width >> mipLevel);
}


unsigned int blp_height(tBLP2Header* pHeader, unsigned int mipLevel)
{
    // Check the mip level
    if (mipLevel >= pHeader->nbMipLevels)
        mipLevel = pHeader->nbMipLevels - 1;

    return (pHeader->height >> mipLevel);
}


tBGRAPixel* blp_convert(FILE* pFile, tBLP2Header* pHeader, unsigned int mipLevel)
{
    // Check the mip level
    if (mipLevel >= pHeader->nbMipLevels)
        mipLevel = pHeader->nbMipLevels - 1;

    // Declarations
    unsigned int width = blp_width(pHeader, mipLevel);
    unsigned int height = blp_height(pHeader, mipLevel);
    uint8_t* pSrc = new uint8_t[pHeader->lengths[mipLevel]];
    tBGRAPixel* pDst = 0;

    // Read the data from the file
    fseek(pFile, pHeader->offsets[mipLevel], SEEK_SET);
    fread((void*) pSrc, sizeof(uint8_t), pHeader->lengths[mipLevel], pFile);
    
    switch (blp_format(pHeader))
    {
        case BLP_FORMAT_PALETTED_NO_ALPHA: pDst = blp_convert_paletted_no_alpha(pSrc, pHeader, width, height); break;
        case BLP_FORMAT_PALETTED_ALPHA_1:  pDst = blp_convert_paletted_alpha1(pSrc, pHeader, width, height); break;
        case BLP_FORMAT_PALETTED_ALPHA_8:  pDst = blp_convert_paletted_alpha8(pSrc, pHeader, width, height); break;
        default:                           break;
    }

    delete[] pSrc;
    
    return pDst;
}


std::string blp_asString(tBLPFormat format)
{
    switch (format)
    {
        case BLP_FORMAT_PALETTED_NO_ALPHA: return "Uncompressed paletted image, no alpha";
        case BLP_FORMAT_PALETTED_ALPHA_1:  return "Uncompressed paletted image, 1-bit alpha";
        case BLP_FORMAT_PALETTED_ALPHA_8:  return "Uncompressed paletted image, 8-bit alpha";
        case BLP_FORMAT_DXT1_NO_ALPHA:     return "DXT1, no alpha";
        case BLP_FORMAT_DXT1_ALPHA_1:      return "DXT1, 1-bit alpha";
        case BLP_FORMAT_DXT3_ALPHA_4:      return "DXT3, 4-bit alpha";
        case BLP_FORMAT_DXT3_ALPHA_8:      return "DXT3, 8-bit alpha";
        case BLP_FORMAT_DXT5_ALPHA_8:      return "DXT5, 8-bit alpha";
        default:                           return "Unknown";
    }
}


tBGRAPixel* blp_convert_paletted_no_alpha(uint8_t* pSrc, tBLP2Header* pHeader, unsigned int width, unsigned int height)
{
    tBGRAPixel* pBuffer = new tBGRAPixel[width * height];
    tBGRAPixel* pDst = pBuffer;
    
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            *pDst = pHeader->palette[*pSrc];
            pDst->a = 0xFF;

            ++pSrc;
            ++pDst;
        }
    }
    
    return pBuffer;
}


tBGRAPixel* blp_convert_paletted_alpha8(uint8_t* pSrc, tBLP2Header* pHeader, unsigned int width, unsigned int height)
{
    tBGRAPixel* pBuffer = new tBGRAPixel[width * height];
    tBGRAPixel* pDst = pBuffer;
    
    uint8_t* pIndices = pSrc;
    uint8_t* pAlpha = pSrc + width * height;
    
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            *pDst = pHeader->palette[*pIndices];
            pDst->a = *pAlpha;

            ++pIndices;
            ++pAlpha;
            ++pDst;
        }
    }
    
    return pBuffer;
}


tBGRAPixel* blp_convert_paletted_alpha1(uint8_t* pSrc, tBLP2Header* pHeader, unsigned int width, unsigned int height)
{
    tBGRAPixel* pBuffer = new tBGRAPixel[width * height];
    tBGRAPixel* pDst = pBuffer;
    
    uint8_t* pIndices = pSrc;
    uint8_t* pAlpha = pSrc + width * height;
    uint8_t counter = 0;
    
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            *pDst = pHeader->palette[*pIndices];
            pDst->a = (*pAlpha & (1 << counter) ? 0xFF : 0x00);

            ++pIndices;
            ++pDst;

            ++counter;
            if (counter == 8)
            {
                ++pAlpha;
                counter = 0;
            }
        }
    }
    
    return pBuffer;
}
