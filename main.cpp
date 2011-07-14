#include "blp.h"
#include <SimpleOpt.h>
#include <iostream>
#include <string>


using namespace std;


/**************************** COMMAND-LINE PARSING ****************************/

// The valid options
enum
{
    OPT_HELP,
    OPT_INFOS,
};


const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,  "-h",      SO_NONE },
    { OPT_HELP,  "--help",  SO_NONE },
    { OPT_INFOS, "-i",      SO_NONE },
    { OPT_INFOS, "--infos", SO_NONE },

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
         << "  - Mip levels: " << blp_nbMipLevels(pHeader) << endl
         << endl;
}


int main(int argc, char** argv)
{
    bool bInfos = false;


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

    // Only support the '--infos' option at the moment
    if (!bInfos)
    {
        cerr << "The conversion of BLP files isn't implemented yet" << endl;
        return -1;
    }
    
    for (unsigned int i = 0; i < args.FileCount(); ++i)
    {
        tBLP2Header header;

        FILE* pFile = fopen(args.File(i), "rb");
        if (!pFile)
        {
            cerr << "Failed to open the file '" << args.File(i) << "'" << endl;
            continue;
        }
    
        if (!blp_processFile(pFile, &header))
        {
            cerr << "Failed to process the file '" << args.File(i) << "'" << endl;
            fclose(pFile);
            continue;
        }

        showInfos(args.File(i), &header);
    
        fclose(pFile);
    }
    
    return 0;
}
