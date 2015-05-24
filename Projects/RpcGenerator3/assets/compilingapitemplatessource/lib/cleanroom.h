#ifndef CLEANROOM_H
#define CLEANROOM_H

#include "icleanroom.h"

class CleanRoomDb;

class CleanRoom : public ICleanRoom
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
private:
    CleanRoomDb *m_Db;
public slots:
    void getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0);
private slots:
    void handlePostCleanRoomDocFinished(ICleanRoomFrontPageDefaultViewRequest *request, bool dbError, bool postCleanRoomDocSuccess_aka_LcbOp, QByteArray dbKey_aka_Sha1);
};

#endif // CLEANROOM_H
