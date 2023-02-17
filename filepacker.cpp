#include "filepacker.h"

fileInfo::fileInfo( QString path, qint64  size, QString hash ) :
    path{path}, size{size}, hash{hash}{}

fileInfo::fileInfo() {}

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

    QFile result( outPath );

    scanDirecory( dirPath );

    if( !saveDirDescription( result) )
    {
        qInfo() << "Unable to open output file";
        return;
    }

    if( !result.open(QIODevice::WriteOnly | QIODevice::Append) )    //binary write
    {
        return;
    }

    QDir dir(dirPath);

    for( qint64 i = 0; i < filesDesc.size(); i++ )
    {
        tick->tick();
        qint64 fileSize = appendFileToResult( dir.filePath(filesDesc[i].path), result );
        if( fileSize == 0 )
        {
            qInfo() << "\nNot all files were packed";
            return;
        }
    }
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

void filePacker::scanDirecory(char* dirPath)
{
    QDirIterator it(dirPath, QStringList() << "*.*", QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
    QDir dir(dirPath);
    while (it.hasNext())
    {
        it.next();
        if(validateFilePath(it.filePath()))
        {
            QString hash = fileChecksum( it.filePath(), QCryptographicHash::Algorithm::Sha1);

            if( !uniqueFile(hash) )
                continue;

            QFile sourceFile(it.filePath());
            qint64 fileSize = sourceFile.size();

            filesDesc.append( fileInfo( dir.relativeFilePath( it.filePath() ), fileSize, hash) );
        }
    }
}

bool filePacker::saveDirDescription(QFile& resultFile)
{
    if( !resultFile.open(QIODevice::WriteOnly | QIODevice::Text) )  // text write
    {
        return false;
    }
    QTextStream stream( &resultFile );
    stream << HEADSTART << '\n';

    for( qint64 i = 0; i < filesDesc.size(); i++ )
    {
        stream << filesDesc[i].path << '\n';
        stream << filesDesc[i].size << '\n';
    }

    stream << HEADEND << '\n';

    resultFile.close();

    return true;
}

bool filePacker::validateFilePath(const QString& filePath)
{
    QFileInfo fi( filePath );
    if( fi.exists() && fi.isFile() )
    {
        return true;
    }
    return false;
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

bool filePacker::uniqueFile(const QString& hash)
{
    for( qint64 i = 0; i < filesDesc.size(); i++ )
    {
        if( filesDesc[i].hash.compare( hash ) == 0 )
        {
            return false;
        }

    }
    return true;
}

/*!
    Returns \c QString with hash created from a \a fileName; empty QString otherwise.
    Source: https://stackoverflow.com/questions/16383392/how-to-get-the-sha-1-md5-checksum-of-a-file-with-qt
*/
QString filePacker::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile sourceFile(fileName);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 10240;

    if (sourceFile.open(QIODevice::ReadOnly))
    {
        char buffer[bufferSize];
        int bytesRead;
        int readSize = qMin(fileSize, bufferSize);

        QCryptographicHash hash(hashAlgorithm);
        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0)
        {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        }

        sourceFile.close();
        return QString(hash.result().toHex());
    }
    return QString();
}

qint64 filePacker::appendFileToResult( const QString& fileName, QFile& resultFile )
{
    QFile inputFile( fileName );
    qint64 fileSize = inputFile.size();
    qint64 startingFileSize = fileSize;
    const qint64 bufferSize = 10240;

    if( !inputFile.open( QIODevice::ReadOnly ) )
    {
        qInfo() << "Unable to open file: " << fileName << '\n';
        return 0;
    }

    char buffer[bufferSize];
    int bytesRead;
    int readSize = qMin(fileSize, bufferSize);

    while( readSize > 0 && (bytesRead = inputFile.read(buffer, readSize) ) > 0 )
    {
        fileSize -= bytesRead;
        resultFile.write( buffer, bytesRead);
        readSize = qMin(fileSize, bufferSize);
    }
    inputFile.close();
    if( fileSize == 0 )
        return startingFileSize;
    else
    {
        qInfo() << "Unable to copy whole file: " << fileName << '\n';
        return 0;
    }
}
