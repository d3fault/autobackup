#ifndef OSIOS_H
#define OSIOS_H

#include <QObject>
#include <QSet>

class QSharedMemory;
class QLocalServer;
class OsiosUiClientConnection;
class QLocalSocket;

class OsiosUiClientConnection;

//Is either a daemon or a daemon relayer for UIs, but behaves identically regardless
class Osios : public QObject
{
    Q_OBJECT
public:
    explicit Osios(QObject *parent = 0);
    ~Osios();
private:
    QSharedMemory *m_SharedMemorySingleSessionEnforcer;
    QLocalServer *m_SerializedActionReceiver;
    QSet<OsiosUiClientConnection*> m_CurrentlyConnectedOsiosUiClients;
signals:
    void thisInstanceOfOsiosIsNotTheFirstOnThisMachine();
    void oisiosLocalServerReadyForIOsiosUiLocalSocketConnection(const QString &);
    void quitRequestedBecauseNoMoreUiClientsConnected();
    void e(const QString &);
public slots:
    void initializeAndStartOsios();
    void handleOsiosUiClientDisconnected(OsiosUiClientConnection *osiosUiClient);
private slots:
    void handleNewOsiosUiClientConnected();
};
class OsiosUiClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit OsiosUiClientConnection(QLocalSocket *clientActual, Osios *serverAndParent);
    ~OsiosUiClientConnection();
private:
    QLocalSocket *m_PointerToSelfActual;
signals:
    void namedDisconnect(OsiosUiClientConnection*);
private slots:
    void handleDisconnected();
};

#endif // OSIOS_H
