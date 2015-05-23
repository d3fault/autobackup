#ifndef CLEANROOM_H
#define CLEANROOM_H

#include "icleanroom.h"

//class ICleanRoomFrontPageDefaultViewRequest;

class CleanRoom : public ICleanRoom
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
public slots:
    void getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0);
};

#endif // CLEANROOM_H
