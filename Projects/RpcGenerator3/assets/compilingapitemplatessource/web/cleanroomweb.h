#include <Wt/WServer>
using namespace Wt;
#ifndef CLEANROOMWEB_H
#define CLEANROOMWEB_H

#include <QObject>
#include <QSharedPointer>

class CleanRoom;

class CleanRoomWeb : public QObject
{
    Q_OBJECT
public:
    explicit CleanRoomWeb(int argc, char *argv[], QObject *parent = 0);
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
