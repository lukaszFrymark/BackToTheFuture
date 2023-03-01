#ifndef FILEPACKER_H
#define FILEPACKER_H
#include <QtCore>
#include "progressticks.h"

#define HEADSTART "BLOBDESCSTART"
#define HEADEND "BLOBDESCEND"

class FileInfo
{
public:
    FileInfo(const QString& path, qint64  size, const QString& hash);
    FileInfo();
    QString path;
    qint64  size;
    QString hash;
};

class FilePacker
{
public:
    FilePacker();
    void pack(const QString& filePath, const QString& outPath);
    void unPack(const QString& filePath, const QString& outDirPath);
    void scanDirecory(const QString& dirPath);
    bool saveDirDescription(QFile& resultFile);
    qint64 getFilesDescFromBlob(QFile& blob);
    bool validateFilePath(const QString& filePath);
    bool validateDirPath(const QString& dirPath);
    bool uniqueFile(const QString& hash);
    QString fileChecksum(const QString& fileName, QCryptographicHash::Algorithm hashAlgorithm);
    qint64 appendFileToResult(const QString& fileName, QFile& resultFile);
    bool makeFileFromBlob(QString& filePath, qint64 fileSize, QFile& blob);

public:
    ProgressTicks tick;
    QString blobFileName;
    QVector<FileInfo> filesDesc;
};

#endif // FILEPACKER_H
