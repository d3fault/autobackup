#ifndef ICLEANROOM_H
#define ICLEANROOM_H

#include <QObject>

class ICleanRoomNewSessionRequest;
class ICleanRoomFrontPageDefaultViewRequest;

class ICleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit ICleanRoom(QObject *parent = 0);
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();
    void newSession(ICleanRoomNewSessionRequest *request);
    virtual void getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0)=0;
};

#endif // ICLEANROOM_H
