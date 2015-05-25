#ifndef CLEANROOMWEB_H
#define CLEANROOMWEB_H

#include <QObject>
#include <QSharedPointer>

namespace Wt
{
    class WServer;
}

class CleanRoom;

class CleanRoomWeb : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomWeb(QObject *parent = 0);
    virtual ~CleanRoomWeb();
private:
    CleanRoom *m_CleanRoom;
    QSharedPointer<WServer> m_WtServer;
signals:
    void initializeAndStartRequested();
private slots:
    void handleCleanRoomReadyForSessions();
};

#endif // CLEANROOMWEB_H
