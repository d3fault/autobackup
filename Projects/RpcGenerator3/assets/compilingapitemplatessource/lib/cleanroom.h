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
    void handleGetLatestCleanRoomDocsFinished(ICleanRoomFrontPageDefaultViewRequest *requestWeWereMerelyForwardingLikeUserDataAkaCookiePointer, bool dbError, bool postCleanRoomDocSuccess_aka_LcbOp, QStringList frontPageDocs); //not rpc generated code
};

#endif // CLEANROOM_H
