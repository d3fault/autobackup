#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>

//class ICleanRoomRequest;
class ICleanRoomNewSessionRequest;
class IFrontPageDefaultViewRequest;

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
    void getFrontPageDefaultView(IFrontPageDefaultViewRequest *request);
    void finishedGettingFrontPageDefaultView(IFrontPageDefaultViewRequest *request, QStringList frontPageDocs);
};

#endif // CLEANROOM_H
