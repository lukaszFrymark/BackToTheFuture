//#include <QCoreApplication>
#include <QtCore>
#include "filepacker.h"

void helpMsg(char* progName)
{
    qInfo() << progName << "[options]" << '\n' <<
        "Options:" << '\n' <<
        "-h | --help                                        Print this help" << '\n' <<
        "-p | --pack {directory_path} {output.path}         Pack files from directory" << '\n' <<
        "-u | --unpack {packed_logs_path} {output.path}     Unpack file into given direcory path" << '\n'
        ;
}

int main(int argc, char *argv[])
{
    filePacker fp;

    if(argc == 1 || QString(argv[1]).compare("-h") == 0)    //No args or '-h'
    {
        helpMsg(argv[0]);
        return 0;
    }
    else if(argc > 3 && (QString(argv[1]).compare("-p") == 0 || QString(argv[1]).compare("-pack") == 0 ))   // -p or -pack with path
    {
        fp.pack( argv[2], argv[3] );
    }

    else if(argc > 3 && (QString(argv[1]).compare("-u") == 0 || QString(argv[1]).compare("-unpack") == 0 ))   // -u or -unpack with paths
    {
        fp.unPack( argv[2], argv[3] );
    }
    else
    {
        qInfo() << "Invalid syntax. Use -help.\n Path to directory/file could be needed." << '\n';
    }

    return 0;
}