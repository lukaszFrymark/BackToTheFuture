//#include <QCoreApplication>
#include <QtCore>
#include "filepacker.h"

void helpMsg(const QString& progName)
{
    qInfo() << progName << "[options]" << '\n' <<
            "Options:" << '\n' <<
            "-h | --help                                        Print this help" << '\n' <<
            "-p | --pack {directory_path} {output.path}         Pack files from directory" << '\n' <<
            "-u | --unpack {packed_logs_path} {output.path}     Unpack file into given direcory path" << '\n'
            ;
}

int main(int argc, char* argv[])
{
    FilePacker fp;

    if (argc == 1 || QString(argv[1]).compare("-h") == 0)   //No args or '-h'
    {
        helpMsg(QString(argv[0]));
        return 0;
    }
    else if (argc > 3 && (QString(argv[1]).compare("-p") == 0 || QString(argv[1]).compare("-pack") == 0))   // -p or -pack with path
    {
        fp.pack(QString(argv[2]), QString(argv[3]));
    }

    else if (argc > 3 && (QString(argv[1]).compare("-u") == 0 || QString(argv[1]).compare("-unpack") == 0))   // -u or -unpack with paths
    {
        fp.unPack(QString(argv[2]), QString(argv[3]));
    }
    else
    {
        qInfo() << "Invalid syntax. Use -help.\n Path to directory/file could be needed." << '\n';
    }

    return 0;
}
