#ifndef _BLP_H_
#define _BLP_H_

#include <stdint.h>
#include <stdio.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

struct tBGRAPixel
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};


// Opaque type representing a BLP file
typedef void* tBLPInfos;


enum tBLPEncoding
{
    BLP_ENCODING_UNCOMPRESSED = 1,
    BLP_ENCODING_DXT = 2,
};


enum tBLPAlphaDepth
{
    BLP_ALPHA_DEPTH_0 = 0,
    BLP_ALPHA_DEPTH_1 = 1,
    BLP_ALPHA_DEPTH_4 = 4,
    BLP_ALPHA_DEPTH_8 = 8,
};


enum tBLPAlphaEncoding
{
    BLP_ALPHA_ENCODING_DXT1 = 0,
    BLP_ALPHA_ENCODING_DXT3 = 1,
    BLP_ALPHA_ENCODING_DXT5 = 7,
};


enum tBLPFormat
{
    BLP_FORMAT_JPEG = 0,

    BLP_FORMAT_PALETTED_NO_ALPHA = (BLP_ENCODING_UNCOMPRESSED << 16) | (BLP_ALPHA_DEPTH_0 << 8),
    BLP_FORMAT_PALETTED_ALPHA_1  = (BLP_ENCODING_UNCOMPRESSED << 16) | (BLP_ALPHA_DEPTH_1 << 8),
    BLP_FORMAT_PALETTED_ALPHA_8  = (BLP_ENCODING_UNCOMPRESSED << 16) | (BLP_ALPHA_DEPTH_8 << 8),

    BLP_FORMAT_DXT1_NO_ALPHA = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_0 << 8) | BLP_ALPHA_ENCODING_DXT1,
    BLP_FORMAT_DXT1_ALPHA_1  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_1 << 8) | BLP_ALPHA_ENCODING_DXT1,
    BLP_FORMAT_DXT3_ALPHA_4  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_4 << 8) | BLP_ALPHA_ENCODING_DXT3,
    BLP_FORMAT_DXT3_ALPHA_8  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_8 << 8) | BLP_ALPHA_ENCODING_DXT3,
    BLP_FORMAT_DXT5_ALPHA_8  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_8 << 8) | BLP_ALPHA_ENCODING_DXT5,
};


tBLPInfos blp_processFile(FILE* pFile);
void blp_release(tBLPInfos blpInfos);

uint8_t blp_version(tBLPInfos blpInfos);
tBLPFormat blp_format(tBLPInfos blpInfos);

unsigned int blp_width(tBLPInfos blpInfos, unsigned int mipLevel=0);
unsigned int blp_height(tBLPInfos blpInfos, unsigned int mipLevel=0);
unsigned int blp_nbMipLevels(tBLPInfos blpInfos);

tBGRAPixel* blp_convert(FILE* pFile, tBLPInfos blpInfos, unsigned int mipLevel=0);

#ifdef __cplusplus
}
#endif

std::string blp_asString(tBLPFormat format);

#endif
