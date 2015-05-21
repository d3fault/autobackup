#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>

class ICleanRoomRequest;

class CleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();
    void newSession(ICleanRoomRequest *request);
    void getFrontPageDefaultView(ICleanRoomRequest *request);
    void finishedGettingFrontPageDefaultView(ICleanRoomRequest *request, QStringList frontPageDocs);
};

#endif // CLEANROOM_H
