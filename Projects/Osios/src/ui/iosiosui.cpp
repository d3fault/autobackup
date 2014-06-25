#include "iosiosui.h"

#include <QLocalSocket>

//Interface for Osios Ui. That is, I implement the ui interface for Osios. Any UI acts as a JIT backend, and additionally a QLocalSocket daemon for additional UIs (the daemon continues running until all UIs disconnect). One "user" profile per ui<-->backend connection is the rule, otherwise the connection is rejected (or perhaps the old one forcibly terminated instead)
IOsiosUi::IOsiosUi(QObject *parent)
    : QObject(parent)
    , m_SerializedActionSender(0)
    , m_ActualUi(0)
{ }
IOsiosUi::~IOsiosUi()
{
    if(m_ActualUi)
        delete m_ActualUi;
}
void IOsiosUi::handleOisiosLocalServerReadyForIOsiosUiLocalSocketConnection(const QString &localServerName)
{
    m_SerializedActionSender = new QLocalSocket(this);
    connect(m_SerializedActionSender, SIGNAL(connected()), this, SLOT(handleLocalSocketConnected()));
    m_SerializedActionSender->connectToServer(localServerName);
}
void IOsiosUi::handleLocalSocketConnected()
{
    //TODOreq: _NOW_ 'present' the ui to the... u.. (actually, maybe some more handshaking first? make sure this is the only "user" connected to this daemon? hell maybe not and just accept their multi-tasking as fine and dandy (sync issues up the wazoo though))

    m_ActualUi = presentUi(); //hasA ui because otherwise i can't put an interface underneath my qmainwindow -_-
}
void IOsiosUi::handleThisUiIsDoneWithOsiosDaemon()
{
    //send/finish-persisting-both-locally-and-remotely/etc, then send 'done/bye' over qlocalsocket before disconnecting

    //TODOreq: osios only ever commits suicide when qlocalserver connections count drops to zero



    //PLACEHOLDER!!!!!!!!!!!!!!!!!!!!!
    if(m_SerializedActionSender)
    {
        if(m_SerializedActionSender->state() == QLocalSocket::ConnectedState)
        {
            m_SerializedActionSender->disconnectFromServer();
            //m_SerializedActionSender->waitForDisconnected(-1); //TODO blah placeholder anyways, async disconnect before delete/zero'ing
        }
        delete m_SerializedActionSender;
        m_SerializedActionSender = 0;
    }
}
