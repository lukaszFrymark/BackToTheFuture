#ifndef FILEPACKER_H
#define FILEPACKER_H
#include <QtCore>
#include "progressticks.h"

#define HEADSTART "BLOBDESCSTART"
#define HEADEND "BLOBDESCEND"

class fileInfo
{
public:
    fileInfo( QString path, qint64  size, QString hash );
    fileInfo();
    QString path;
    qint64  size;
    QString hash;
};

class filePacker
{
public:
    filePacker();
    void pack(char* filePath, char* outPath);
    void unPack(char* filePath, char* outDirPath);
    void scanDirecory(char* dirPath);
    bool saveDirDescription(QFile& resultFile);
    bool validateFilePath(char* filePath);
    bool validateFilePath(const QString& filePath);
    bool validateDirPath(char* dirPath);
    QString fileChecksum(const QString& fileName, QCryptographicHash::Algorithm hashAlgorithm);
    qint64 appendFileToResult( const QString& fileName, QFile& resultFile );
    bool uniqueFile(const QString& hash);

public:
    ProgressTicks* tick;
    QString blobFileName;
    QVector<fileInfo> filesDesc;

};

#endif // FILEPACKER_H
