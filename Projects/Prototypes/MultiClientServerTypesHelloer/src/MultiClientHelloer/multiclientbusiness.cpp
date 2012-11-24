#include "multiclientbusiness.h"

MultiClientBusiness::MultiClientBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_ClientHelloer, SIGNAL(connectionHasBeenHelloedAndIsReadyForAction(QIODevice*,quint32)), this, SLOT(newConnectionPassedHelloPhase(QIODevice*,quint32)));
    connect(&m_ClientHelloer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
}
void MultiClientBusiness::addSslClient()
{
    m_ClientHelloer.addSslClient();
}
void MultiClientBusiness::newConnectionPassedHelloPhase(QIODevice *theConnection, quint32 clientId)
{
    m_ActiveConnectionIdsByIODevice.insert(theConnection, clientId);
    connect(theConnection, SIGNAL(readyRead()), this, SLOT(serverSentUsData()));
    //TODOreq: see server todos
}
void MultiClientBusiness::serverSentUsData()
{
    QIODevice *serverSocket = static_cast<QIODevice*>(sender());

    if(serverSocket)
    {
        quint32 connectionId = m_ActiveConnectionIdsByIODevice.value(serverSocket, 0);
        if(connectionId > 0)
        {
            //read the message into a recycled container or whatever the fuck
            //set it's connectionId
            //process it


            //earlier (not in here):
            //deliver() received, send to multi client using connectionId
        }
        else
        {
            //server doesn't have an ID? Somehow got out of helloer or something without one? should never happen
        }
    }
    else
    {
        //cast failed wtf invalid sender()? should never happen
    }
}
