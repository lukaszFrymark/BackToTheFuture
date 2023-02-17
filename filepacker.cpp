#include "filepacker.h"

filePacker::filePacker()
{
    tick = new ProgressTicks();
}

void filePacker::pack(char* dirPath, char* outPath)
{
    qInfo() << "Packing direcory content:" << dirPath << '\n' <<
               "Into file:" << outPath << '\n';

    if( !validateDirPath(dirPath) )
        return;

    QDirIterator it(dirPath, QStringList() << "*.*", QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
    while (it.hasNext())
        qDebug() << it.next();

    qInfo() << "\nPacking direcory content finished";
}

void filePacker::unPack(char* filePath, char* outDirPath)
{
    qInfo() << "Unpacking file content:" << filePath << '\n' <<
               "Into direcory:" << outDirPath << '\n';

    if( !validateFilePath(filePath) || !validateDirPath(outDirPath) )
        return;

    for (int i=0; i<10; i++)
    {
        tick->tick();
        QObject().thread()->usleep(1000*1000);
    }
    qInfo() << "\nUnpacking direcory content finished";
}

bool filePacker::validateFilePath(char* filePath)
{
    QFileInfo fi( filePath );
    if( fi.exists() && fi.isFile() )
    {
        return true;
    }
    else
    {
        qInfo() << "Given path is not a file:" << filePath;
        return false;
    }
}

bool filePacker::validateDirPath(char* dirPath)
{
    QFileInfo fi( dirPath );
    if( fi.exists() && fi.isDir() )
    {
        return true;
    }
    else
    {
        qInfo() << "Given path is not a directory:" << dirPath;
        return false;
    }
}
