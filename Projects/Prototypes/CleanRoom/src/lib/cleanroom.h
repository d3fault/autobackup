#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>

class CleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();

    void handleNewSession(ICleanRoomSession *session);
    void handleSessionRequestRequested(ICleanRoomSessionRequest *sessionRequest);

    void getFrontPageDefaultViewBegin(FrontPageDefaultViewCleanRoomSessionRequest *request);
    void handleFinishedGettingFrontPageDefaultViewFromDb(FrontPageDefaultViewCleanRoomSessionRequest *request, QList<QString> frontPageDocs);
};

#endif // CLEANROOM_H
