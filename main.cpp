#include "blp.h"
#include <SimpleOpt.h>
#include <FreeImage.h>
#include <memory.h>
#include <iostream>
#include <string>


using namespace std;


/**************************** COMMAND-LINE PARSING ****************************/

// The valid options
enum
{
    OPT_HELP,
    OPT_INFOS,
    OPT_DEST,
};


const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,  "-h",      SO_NONE },
    { OPT_HELP,  "--help",  SO_NONE },
    { OPT_INFOS, "-i",      SO_NONE },
    { OPT_INFOS, "--infos", SO_NONE },
    { OPT_DEST,  "-o",      SO_REQ_SEP },
    { OPT_DEST,  "--dest",  SO_REQ_SEP },

    SO_END_OF_OPTIONS
};


/********************************** FUNCTIONS *********************************/

void showUsage(const std::string& strApplicationName)
{
    cout << "BLPConverter" << endl
         << "Usage: " << strApplicationName << " [options] <blp_filename> [<blp_filename> ... <blp_filename>]" << endl
         << endl;
}


void showInfos(const std::string& strFileName, tBLP2Header* pHeader)
{
    cout << endl
         << "Infos about '" << strFileName << "':" << endl
         << "  - Format:     " << blp_asString(blp_format(pHeader)) << endl
         << "  - Dimensions: " << pHeader->width << "x" << pHeader->height << endl
         << "  - Mip levels: " << (unsigned int) pHeader->nbMipLevels << endl
         << endl;
}


int main(int argc, char** argv)
{
    bool         bInfos             = false;
    string       strOutputFolder    = "./";
    unsigned int mipLevel           = 0;
    unsigned int nbImagesTotal      = 0;
    unsigned int nbImagesConverted  = 0;


    // Parse the command-line parameters
    CSimpleOpt args(argc, argv, COMMAND_LINE_OPTIONS);
    while (args.Next())
    {
        if (args.LastError() == SO_SUCCESS)
        {
            switch (args.OptionId())
            {
                case OPT_HELP:
                    showUsage(argv[0]);
                    return 0;
                
                case OPT_INFOS:
                    bInfos = true;
                    break;

                case OPT_DEST:
                    strOutputFolder = args.OptionArg();
                    if (strOutputFolder.at(strOutputFolder.size() - 1) != '/')
                        strOutputFolder += "/";
                    break;
            }
        }
        else
        {
            cerr << "Invalid argument: " << args.OptionText() << endl;
            return -1;
        }
    }

    if (args.FileCount() == 0)
    {
        cerr << "No BLP file specified" << endl;
        return -1;
    }

    
    // Initialise FreeImage
    FreeImage_Initialise(true);


    // Process the files
    for (unsigned int i = 0; i < args.FileCount(); ++i)
    {
        ++nbImagesTotal;
        
        string strInFileName = args.File(i);
        string strOutFileName = strInFileName.substr(0, strInFileName.size() - 3) + "png";
        
        size_t offset = strOutFileName.find_last_of("/");
        if (offset != string::npos)
            strOutFileName = strOutFileName.substr(offset + 1);
        
        FILE* pFile = fopen(strInFileName.c_str(), "rb");
        if (!pFile)
        {
            cerr << "Failed to open the file '" << strInFileName << "'" << endl;
            continue;
        }

        tBLP2Header* pHeader = blp_processFile(pFile);
        if (!pHeader)
        {
            cerr << "Failed to process the file '" << strInFileName << "'" << endl;
            fclose(pFile);
            continue;
        }

        if (!bInfos)
        {
            tBGRAPixel* pData = blp_convert(pFile, pHeader, mipLevel);
            if (pData)
            {
                unsigned int width = blp_width(pHeader, mipLevel);
                unsigned int height = blp_height(pHeader, mipLevel);
                
                FIBITMAP* pImage = FreeImage_Allocate(width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000);
                if (pImage)
                {
                    tBGRAPixel* pSrc = pData;
                    
                    for (unsigned int y = 0; y < height; ++y)
                    {
                        BYTE* pLine = FreeImage_GetScanLine(pImage, y);
                        memcpy(pLine, pSrc, width * sizeof(tBGRAPixel));
                        
                        pSrc += width;
                    }
                    
                    if (FreeImage_Save(FIF_PNG, pImage, (strOutputFolder + strOutFileName).c_str(), 0))
                    {
                        cerr << strInFileName << ": OK" << endl;
                        ++nbImagesConverted;
                    }
                    else
                    {
                        cerr << strInFileName << ": Failed to save the image" << endl;
                    }
                    
                    FreeImage_Unload(pImage);
                }
                else
                {
                    cerr << strInFileName << ": Failed to allocate memory" << endl;
                }

                delete[] pData;
            }
            else
            {
                cerr << strInFileName << ": Unsupported format" << endl;
            }
        }
        else
        {
            showInfos(args.File(i), pHeader);
        }
    
        fclose(pFile);

        delete pHeader;
    }
    
    // Cleanup
    FreeImage_DeInitialise();
    
    return 0;
}
