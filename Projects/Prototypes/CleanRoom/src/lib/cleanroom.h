#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>

class ICleanRoomNewSessionRequest;
class ICleanRoomFrontPageDefaultViewRequest;

class CleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();
    void newSession(ICleanRoomNewSessionRequest *request);
    void getFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, double someArg0);
    void finishedGettingFrontPageDefaultView(ICleanRoomFrontPageDefaultViewRequest *request, QStringList frontPageDocs);
};

#endif // CLEANROOM_H
