#ifndef COPYOPERATION_H
#define COPYOPERATION_H

#include <QString>

struct CopyOperation
{
    enum CopyOperationTypes { CopyFallback, StillImage, Audio, RawPcm22050s16leAudio, Video };

    QString SourceFilePath;
    QString DestinationFilePath;
    CopyOperationTypes CopyOperationType;
};

#endif // COPYOPERATION_H
