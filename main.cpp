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
    OPT_FORMAT,
    OPT_MIP_LEVEL,
};


const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,      "-h",         SO_NONE },
    { OPT_HELP,      "--help",     SO_NONE },
    { OPT_INFOS,     "-i",         SO_NONE },
    { OPT_INFOS,     "--infos",    SO_NONE },
    { OPT_DEST,      "-o",         SO_REQ_SEP },
    { OPT_DEST,      "--dest",     SO_REQ_SEP },
    { OPT_FORMAT,    "-f",         SO_REQ_SEP },
    { OPT_FORMAT,    "--format",   SO_REQ_SEP },
    { OPT_MIP_LEVEL, "-m",         SO_REQ_SEP },
    { OPT_MIP_LEVEL, "--miplevel", SO_REQ_SEP },

    SO_END_OF_OPTIONS
};


/********************************** FUNCTIONS *********************************/

void showUsage(const std::string& strApplicationName)
{
    cout << "BLPConverter" << endl
         << endl
         << "Usage: " << strApplicationName << " [options] <blp_filename> [<blp_filename> ... <blp_filename>]" << endl
         << endl
         << "Options:" << endl
         << "  --help, -h:      Display this help" << endl
         << "  --infos, -i:     Display informations about the BLP file(s) (no conversion)" << endl
         << "  --dest, -o:      Folder where the converted image(s) must be written to (default: './')" << endl
         << "  --format, -f:    'png' or 'tga' (default: png)" << endl
         << "  --miplevel, -m:  The specific mip level to convert (default: 0, the bigger one)" << endl
         << endl;
}


void showInfos(const std::string& strFileName, tBLPInfos blpInfos)
{
    cout << endl
         << "Infos about '" << strFileName << "':" << endl
         << "  - Version:    BLP" << blp_version(blpInfos) << endl
         << "  - Format:     " << blp_asString(blp_format(blpInfos)) << endl
         << "  - Dimensions: " << blp_width(blpInfos) << "x" << blp_height(blpInfos) << endl
         << "  - Mip levels: " << blp_nbMipLevels(blpInfos) << endl
         << endl;
}


int main(int argc, char** argv)
{
    bool         bInfos             = false;
    string       strOutputFolder    = "./";
    string       strFormat          = "png";
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

                case OPT_FORMAT:
                    strFormat = args.OptionArg();
                    if (strFormat != "tga")
                        strFormat = "png";
                    break;

                case OPT_MIP_LEVEL:
                    mipLevel = atoi(args.OptionArg());
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
        string strOutFileName = strInFileName.substr(0, strInFileName.size() - 3) + strFormat;

        size_t offset = strOutFileName.find_last_of("/\\");
        if (offset != string::npos)
            strOutFileName = strOutFileName.substr(offset + 1);

        FILE* pFile = fopen(strInFileName.c_str(), "rb");
        if (!pFile)
        {
            cerr << "Failed to open the file '" << strInFileName << "'" << endl;
            continue;
        }

        tBLPInfos blpInfos = blp_processFile(pFile);
        if (!blpInfos)
        {
            cerr << "Failed to process the file '" << strInFileName << "'" << endl;
            fclose(pFile);
            continue;
        }

        if (!bInfos)
        {
            tBGRAPixel* pData = blp_convert(pFile, blpInfos, mipLevel);
            if (pData)
            {
                unsigned int width = blp_width(blpInfos, mipLevel);
                unsigned int height = blp_height(blpInfos, mipLevel);

                FIBITMAP* pImage = FreeImage_Allocate(width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000);
                if (pImage)
                {
                    tBGRAPixel* pSrc = pData + (height - 1) * width;

                    for (unsigned int y = 0; y < height; ++y)
                    {
                        BYTE* pLine = FreeImage_GetScanLine(pImage, y);
                        memcpy(pLine, pSrc, width * sizeof(tBGRAPixel));

                        pSrc -= width;
                    }

                    if (FreeImage_Save((strFormat == "tga" ? FIF_TARGA : FIF_PNG), pImage, (strOutputFolder + strOutFileName).c_str(), 0))
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
            showInfos(args.File(i), blpInfos);
        }

        fclose(pFile);

        blp_release(blpInfos);
    }

    // Cleanup
    FreeImage_DeInitialise();

    return 0;
}
