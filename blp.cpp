#include "blp.h"
#include <string.h>


bool blp_processFile(FILE* pFile, tBLP2Header* pHeader)
{
    fseek(pFile, 0, SEEK_SET);
    fread((void*) pHeader, sizeof(uint8_t), 4, pFile);

    if (strncmp((char*) pHeader->magic, "BLP2", 4) != 0)
        return false;

    fread((void*) &pHeader->type, sizeof(tBLP2Header) - 4 * sizeof(uint8_t), 1, pFile);
    
    return true;
}


tBLPFormat blp_format(tBLP2Header* pHeader)
{
    if (pHeader->encoding == BLP_ENCODING_UNCOMPRESSED)
        return tBLPFormat((pHeader->encoding << 16) | (pHeader->alphaDepth << 8));
    
    return tBLPFormat((pHeader->encoding << 16) | (pHeader->alphaDepth << 8) | pHeader->alphaEncoding);
}


unsigned int blp_nbMipLevels(tBLP2Header* pHeader)
{
    unsigned int nb = 0;
    
    while ((pHeader->offsets[nb] != 0) && (nb < 16))
        ++nb;

    return nb;
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
