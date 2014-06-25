#include "osios.h"

#include <QStringList>
#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QCryptographicHash>
#include <QDateTime>
#include <QSharedMemory>
#include <QMessageBox>

#define OSIOS_DAEMON_KEY "OsiosDaemon"

//TODOreq: if the connection fails, we should loop the "are other instances open?" logic so that the race condition where our shared memory create check fails but we don't connect before the daemon itself closes is accounted for. in that case, we become the daemon. also TODOreq related to that, we need to be completely finished with disk i/o (persistence) before stopping listening, and we should stop listening before detaching from the shared memory also
Osios::Osios(QObject *parent)
    : QObject(parent)
    , m_SerializedActionReceiver(0)
{ }
Osios::~Osios()
{
    qDeleteAll(m_CurrentlyConnectedOsiosUiClients.begin(), m_CurrentlyConnectedOsiosUiClients.end());
    if(m_SerializedActionReceiver)
    {
        if(m_SerializedActionReceiver->isListening())
            m_SerializedActionReceiver->close(); //TODOreq: make sure destructor is never called until all connections already disconnect
        delete m_SerializedActionReceiver;
    }
}
void Osios::initializeAndStartOsios()
{
    //QStringList argz = QCoreApplication::arguments();
    //QByteArray myFilePathKeyHexBA = QCryptographicHash::hash(argz.at(0), QCryptographicHash::Sha1).toHex(); //TODOreq: use data dir for key generation, but i don't think qsettings is safe to access with many processes simultaneously. OLD: instead of using the filepath, applications could "become singletons" based on the provided "user profile" (if just a name) or similar "profile directory" (like "data" dir)... once I design/code that far. I guess right now to KISS, my data dir is the dir that the binary lives in? idk haven't thought about it yet
    //QString myFilePathKeyHex(myFilePathKeyHexBA);

    m_SharedMemorySingleSessionEnforcer = new QSharedMemory(OSIOS_DAEMON_KEY, this);
    if(m_SharedMemorySingleSessionEnforcer->create(1, QSharedMemory::ReadOnly))
    {
        //first instance on this machine, start local server
        m_SerializedActionReceiver = new QLocalServer(this);
        connect(m_SerializedActionReceiver, SIGNAL(newConnection()), this, SLOT(handleNewOsiosUiClientConnected()));
        if(!m_SerializedActionReceiver->listen(OSIOS_DAEMON_KEY))
        {
            emit e("failed to listen to localserver on: " OSIOS_DAEMON_KEY);
            delete m_SerializedActionReceiver;
            m_SerializedActionReceiver = 0;
            m_SharedMemorySingleSessionEnforcer->detach();
            return;
        }
    }
    else if(m_SharedMemorySingleSessionEnforcer->attach(QSharedMemory::ReadWrite))
    {
        //not first instance, which means the ui will connect to the already existing one via qlocalsocket
        //so we tell our process to clean up this [unused] instance of osios
    }
    else
    {
        emit e("shared memory failed to create OR attach (should never happen): " OSIOS_DAEMON_KEY);
        return; //TODOreq: request quit?
    }
    //TODOoptional: building on the above single session checking, different "user profile" instances can (should?) still jit-relay the data to a single session via qsharedmemory and qsystemsemaphore message passing (or qlocalsocket), but that assumes that all user profiles share the same data dir (should they? maybe since everything is all on one shared dht)

    //either create or attach succeeded if we get here

    //not first instance on this machine, do nothing and this INSTANCE of this class (in this process) will be skipped. the other one will be connected to by the UI
#if 0
    QMessageBox messageBox(tr("Error: Only one instance allowed at a time"), tr("There is already an instance of Osios running. If you recently closed it, it may still be trying to save your data in the background. Please wait a few moments before trying again"), QMessageBox::Ok); //TODOoptional: a "quit while watching that data saving finish up" type of quit
    messageBox.exec();
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
    return;
#endif

    //either way, we eventually want to:
    emit oisiosLocalServerReadyForIOsiosUiLocalSocketConnection(OSIOS_DAEMON_KEY);
}
void Osios::handleOsiosUiClientDisconnected(OsiosUiClientConnection *osiosUiClient)
{
    m_CurrentlyConnectedOsiosUiClients.remove(osiosUiClient);
    delete osiosUiClient;

    if(m_CurrentlyConnectedOsiosUiClients.isEmpty())
    {
        //now that every ui client has disconnected, we can ACTUALLY quit
        emit quitRequestedBecauseNoMoreUiClientsConnected();
    }
}
void Osios::handleNewOsiosUiClientConnected()
{
    while(m_SerializedActionReceiver->hasPendingConnections())
    {
        m_CurrentlyConnectedOsiosUiClients.insert(new OsiosUiClientConnection(m_SerializedActionReceiver->nextPendingConnection(), this));

    }
}
OsiosUiClientConnection::OsiosUiClientConnection(QLocalSocket *clientActual, Osios *serverAndParent)
    : QObject(serverAndParent)
  , m_PointerToSelfActual(clientActual)
{
  connect(clientActual, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));
  connect(this, SIGNAL(namedDisconnect(OsiosUiClientConnection*)), serverAndParent, SLOT(handleOsiosUiClientDisconnected(OsiosUiClientConnection*)));
}
OsiosUiClientConnection::~OsiosUiClientConnection()
{
    delete m_PointerToSelfActual;
}
void OsiosUiClientConnection::handleDisconnected()
{
    emit namedDisconnect(this); //fuck sender()
}

