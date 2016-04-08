#ifndef _BLP_INTERNAL_H_
#define _BLP_INTERNAL_H_

#include <stdint.h>
#include <stdio.h>
#include <string>


// A description of the BLP1 format can be found in the file doc/MagosBlpFormat.txt
struct tBLP1Header
{
    uint8_t     magic[4];       // Always 'BLP1'
    uint32_t    type;           // 0: JPEG, 1: palette
    uint32_t    flags;          // 8: Alpha
    uint32_t    width;          // In pixels, power-of-two
    uint32_t    height;
    uint32_t    alphaEncoding;  // 3, 4: Alpha list, 5: Alpha from palette
    uint32_t    flags2;         // Unused
    uint32_t    offsets[16];
    uint32_t    lengths[16];
};


// Additional informations about a BLP1 file
struct tBLP1Infos
{
    uint8_t nbMipLevels;    // The number of mip levels

    union {
        tBGRAPixel  palette[256];   // 256 BGRA colors

        struct {
            uint32_t headerSize;
            uint8_t* header;        // Shared between all mipmap levels
        } jpeg;
    };
};


// A description of the BLP2 format can be found on Wikipedia: http://en.wikipedia.org/wiki/.BLP
struct tBLP2Header
{
    uint8_t     magic[4];       // Always 'BLP2'
    uint32_t    type;           // 0: JPEG, 1: see encoding
    uint8_t     encoding;       // 1: Uncompressed, 2: DXT compression, 3: Uncompressed BGRA
    uint8_t     alphaDepth;     // 0, 1, 4 or 8 bits
    uint8_t     alphaEncoding;  // 0: DXT1, 1: DXT3, 7: DXT5

    union {
        uint8_t     hasMipLevels;   // In BLP file: 0 or 1
        uint8_t     nbMipLevels;    // For convenience, replaced with the number of mip levels
    };

    uint32_t    width;          // In pixels, power-of-two
    uint32_t    height;
    uint32_t    offsets[16];
    uint32_t    lengths[16];
    tBGRAPixel  palette[256];   // 256 BGRA colors
};


// Internal representation of any BLP header
struct tInternalBLPInfos
{
    uint8_t version;     // 1 or 2

    union {
        struct {
            tBLP1Header header;
            tBLP1Infos  infos;
        } blp1;

        tBLP2Header blp2;
    };
};

#endif
