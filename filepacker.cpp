#include "filepacker.h"

FileInfo::FileInfo(QString path, qint64  size, QString hash) :
    path{path}, size{size}, hash{hash} {}

FileInfo::FileInfo() {}

FilePacker::FilePacker()
{
    ProgressTicks tick;
}

void FilePacker::pack(char* dirPath, char* outPath)
{
    qInfo() << "Packing direcory content:" << dirPath << '\n' <<
            "Into file:" << outPath << '\n';

    if (!validateDirPath(dirPath))
        return;

    QFile result(outPath);

    scanDirecory(dirPath);

    if (!saveDirDescription(result))
    {
        qInfo() << "Unable to open output file";
        return;
    }

    if (!result.open(QIODevice::WriteOnly | QIODevice::Append))       //binary write
    {
        return;
    }

    QDir dir(dirPath);

    for (qint64 i = 0; i < filesDesc.size(); i++)
    {
        tick.tick();
        qint64 fileSize = appendFileToResult(dir.filePath(filesDesc[i].path), result);
        if (fileSize == 0)
        {
            qInfo() << "\nNot all files were packed";
            return;
        }
    }
    qInfo() << "\nPacking direcory content finished";
}

void FilePacker::unPack(char* filePath, char* outDirPath)
{
    qInfo() << "Unpacking file content:" << filePath << '\n' <<
            "Into direcory:" << outDirPath << '\n';

    if (!validateFilePath(filePath) || !validateDirPath(outDirPath))
        return;

    QFile blob(filePath);
    qint64 startOfBinaryData = getFilesDescFromBlob(blob);
    if (startOfBinaryData > 0)
    {
        blob.seek(startOfBinaryData);    // will move QFile pointer to binary part
    }

    QDir outDir(outDirPath);

    for (qint64 i = 0; i < filesDesc.size(); i++)
    {
        QString outFilePath = outDir.filePath(filesDesc[i].path);
        QDir outFileDir = QFileInfo(outFilePath).absoluteDir();
        if (!QFileInfo::exists(outFileDir.absolutePath()))
        {
            outFileDir.mkpath(outFileDir.absolutePath());
        }

        tick.tick();
        makeFileFromBlob(outFilePath, filesDesc[i].size, blob);     // will move QFile pointer to next file part
    }

    qInfo() << "\nUnpacking direcory content finished";
}

void FilePacker::scanDirecory(char* dirPath)
{
    QDirIterator it(dirPath, QStringList() << "*.*", QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
    QDir dir(dirPath);
    while (it.hasNext())
    {
        it.next();
        if (validateFilePath(it.filePath()))
        {
            QString hash = fileChecksum(it.filePath(), QCryptographicHash::Algorithm::Sha1);

            if (!uniqueFile(hash))
                continue;

            QFile sourceFile(it.filePath());
            qint64 fileSize = sourceFile.size();

            filesDesc.append(FileInfo(dir.relativeFilePath(it.filePath()), fileSize, hash));
        }
    }
}

bool FilePacker::saveDirDescription(QFile& resultFile)
{
    if (!resultFile.open(QIODevice::WriteOnly | QIODevice::Text))     // text write
    {
        return false;
    }
    QTextStream stream(&resultFile);
    stream << HEADSTART << '\n';

    for (qint64 i = 0; i < filesDesc.size(); i++)
    {
        stream << filesDesc[i].path << '\n';
        stream << filesDesc[i].size << '\n';
    }

    stream << HEADEND << '\n';

    resultFile.close();

    return true;
}

qint64 FilePacker::getFilesDescFromBlob(QFile& blob)
{
    QTextStream in(&blob);

    if (!blob.open(QIODevice::ReadOnly))       //text read
    {
        return 0;
    }

    if (QString(in.readLine()).compare(HEADSTART) != 0)
    {
        qInfo() << "Wrong input file format";
        return 0;
    }

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.compare(HEADEND) == 0)
        {
            return in.pos() + sizeof('\n') - sizeof(char);       // position plus newline  minus \0 char
        }
        bool converted = false;
        qint64  size = QString(in.readLine()).toLongLong(&converted, 10);
        filesDesc.append(FileInfo(line, converted ? size : 0, QString()));
    }

    return 0;
}

bool FilePacker::validateFilePath(const QString& filePath)
{
    QFileInfo fi(filePath);
    if (fi.exists() && fi.isFile())
    {
        return true;
    }
    return false;
}

bool FilePacker::validateFilePath(char* filePath)
{
    QFileInfo fi(filePath);
    if (fi.exists() && fi.isFile())
    {
        return true;
    }
    else
    {
        qInfo() << "Given path is not a file:" << filePath;
        return false;
    }
}

bool FilePacker::validateDirPath(char* dirPath)
{
    QFileInfo fi(dirPath);
    if (fi.exists() && fi.isDir())
    {
        return true;
    }
    else
    {
        qInfo() << "Given path is not a directory:" << dirPath;
        return false;
    }
}

bool FilePacker::uniqueFile(const QString& hash)
{
    for (qint64 i = 0; i < filesDesc.size(); i++)
    {
        if (filesDesc[i].hash.compare(hash) == 0)
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
QString FilePacker::fileChecksum(const QString& fileName, QCryptographicHash::Algorithm hashAlgorithm)
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

qint64 FilePacker::appendFileToResult(const QString& fileName, QFile& resultFile)
{
    QFile inputFile(fileName);
    qint64 fileSize = inputFile.size();
    qint64 startingFileSize = fileSize;
    const qint64 bufferSize = 10240;

    if (!inputFile.open(QIODevice::ReadOnly))
    {
        qInfo() << "Unable to open file: " << fileName << '\n';
        return 0;
    }

    char buffer[bufferSize];
    int bytesRead;
    int readSize = qMin(fileSize, bufferSize);

    while (readSize > 0 && (bytesRead = inputFile.read(buffer, readSize)) > 0)
    {
        fileSize -= bytesRead;
        resultFile.write(buffer, bytesRead);
        readSize = qMin(fileSize, bufferSize);
    }
    inputFile.close();
    if (fileSize == 0)
        return startingFileSize;
    else
    {
        qInfo() << "Unable to copy whole file: " << fileName << '\n';
        return 0;
    }
}

bool FilePacker::makeFileFromBlob(QString& filePath, qint64 fileSize, QFile& blob)
{
    QFile resultFile(filePath);
    const qint64 bufferSize = 10240;
    qint64 startingFileSize = fileSize;

    if (!resultFile.open(QIODevice::WriteOnly))
    {
        qInfo() << "Unable to create file: " << filePath << '\n';
        return false;
    }

    char buffer[bufferSize];
    int bytesRead;
    int readSize = qMin(startingFileSize, bufferSize);

    while (readSize > 0 && (bytesRead = blob.read(buffer, readSize)) > 0)
    {
        fileSize -= bytesRead;
        resultFile.write(buffer, bytesRead);
        readSize = qMin(fileSize, bufferSize);
    }

    resultFile.close();

    if (fileSize == 0)
        return true;
    else
    {
        qInfo() << "Unable to copy whole file: " << filePath << '\n';
        return false;
    }
}
