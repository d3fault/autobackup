#ifndef CLEANROOMDB_H
#define CLEANROOMDB_H

#include <QObject>

class CleanRoomSession;

class CleanRoomDb : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomDb(QObject *parent = 0);
    void getFrontPageDefaultViewBegin(CleanRoomSession *session);
signals:
    void finishedGettingFrontPageDefaultView(CleanRoomSession *session, QList<QString> frontPageEntries);
public slots:
    void handleFinishedGettingFrontPageDefaultView(CleanRoomSession *session, QList<QString> frontPageEntries);
};

#endif // CLEANROOMDB_H
