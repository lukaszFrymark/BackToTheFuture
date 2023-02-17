#ifndef FILEPACKER_H
#define FILEPACKER_H
#include <QtCore>
#include "progressticks.h"

class filePacker
{
public:
    filePacker();
    void pack(char* filePath, char* outPath);
    void unPack(char* filePath, char* outDirPath);
    bool validateFilePath(char* filePath);
    bool validateDirPath(char* dirPath);

public:
    ProgressTicks* tick;
};

#endif // FILEPACKER_H
