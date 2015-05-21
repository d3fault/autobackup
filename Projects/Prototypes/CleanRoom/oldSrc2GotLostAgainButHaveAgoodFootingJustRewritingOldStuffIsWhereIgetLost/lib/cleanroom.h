#ifndef CLEANROOM_H
#define CLEANROOM_H

#include <QObject>
#include <QSharedPointer>

class CleanRoomDb;
class CleanRoomSession;
class ICleanRoomFrontEndResponder;
class ICleanRoomSessionRequest;
class FrontPageDefaultViewCleanRoomSessionRequest;

class CleanRoom : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoom(QObject *parent = 0);
private:
    CleanRoomDb *m_Db;
    QList<QSharedPointer<CleanRoomSession> > m_Sessions;
signals:
    void readyForSessions();
public slots:
    void initializeAndStart();

    void startNewSession(ICleanRoomFrontEndResponder *cleanRoomFrontEndResponder);
    void handleSessionRequestRequested(ICleanRoomSessionRequest *sessionRequest);

    void getFrontPageDefaultViewBegin(CleanRoomSession *session);
    void handleFinishedGettingFrontPageDefaultViewFromDb(CleanRoomSession*session, QList<QString> frontPageDocs);
};

#endif // CLEANROOM_H
