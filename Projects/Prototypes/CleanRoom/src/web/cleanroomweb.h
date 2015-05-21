#ifndef CLEANROOMWEB_H
#define CLEANROOMWEB_H

#include <QObject>

class CleanRoom;

class CleanRoomWeb : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomWeb(QObject *parent = 0);
private:
    CleanRoom *m_CleanRoom;
private slots:
    void handleCleanRoomReadyForSessions();
};

#endif // CLEANROOMWEB_H
