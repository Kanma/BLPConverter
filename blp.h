#ifndef _BLP_H_
#define _BLP_H_

#include <stdint.h>
#include <stdio.h>
#include <string>


// A description of the BLP format can be found on Wikipedia: http://en.wikipedia.org/wiki/.BLP

struct tBLP2Header
{
    uint8_t     magic[4];       // Always 'BLP2'
    uint32_t    type;           // 0: JPEG, 1: see encoding
    uint8_t     encoding;       // 1: Uncompressed, 2: DXT compression
    uint8_t     alphaDepth;     // 0, 1, 4 or 8 bits
    uint8_t     alphaEncoding;  // 0: DXT1, 1: DXT3, 7: DXT5
    uint8_t     hasMipLevels;
    uint32_t    width;          // In pixels, power-of-two
    uint32_t    height;
    uint32_t    offsets[16];
    uint32_t    lengths[16];
    uint32_t    palette[256];   // 256 BGRA colors
};


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
    BLP_FORMAT_PALETTED_NO_ALPHA = (BLP_ENCODING_UNCOMPRESSED << 16) | (BLP_ALPHA_DEPTH_0 << 8),
    BLP_FORMAT_PALETTED_ALPHA_1  = (BLP_ENCODING_UNCOMPRESSED << 16) | (BLP_ALPHA_DEPTH_1 << 8),
    BLP_FORMAT_PALETTED_ALPHA_8  = (BLP_ENCODING_UNCOMPRESSED << 16) | (BLP_ALPHA_DEPTH_8 << 8),

    BLP_FORMAT_DXT1_NO_ALPHA = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_0 << 8) | BLP_ALPHA_ENCODING_DXT1,
    BLP_FORMAT_DXT1_ALPHA_1  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_1 << 8) | BLP_ALPHA_ENCODING_DXT1,
    BLP_FORMAT_DXT3_ALPHA_4  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_4 << 8) | BLP_ALPHA_ENCODING_DXT3,
    BLP_FORMAT_DXT3_ALPHA_8  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_8 << 8) | BLP_ALPHA_ENCODING_DXT3,
    BLP_FORMAT_DXT5_ALPHA_8  = (BLP_ENCODING_DXT << 16) | (BLP_ALPHA_DEPTH_8 << 8) | BLP_ALPHA_ENCODING_DXT5,
};


bool blp_processFile(FILE* pFile, tBLP2Header* pHeader);

tBLPFormat blp_format(tBLP2Header* pHeader);

unsigned int blp_nbMipLevels(tBLP2Header* pHeader);

std::string blp_asString(tBLPFormat format);

#endif
