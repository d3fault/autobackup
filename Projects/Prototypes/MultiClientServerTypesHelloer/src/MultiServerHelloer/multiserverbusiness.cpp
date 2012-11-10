#include "multiserverbusiness.h"

MultiServerBusiness::MultiServerBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_Helloer, SIGNAL(connectionHasBeenHelloedAndIsReadyForAction(QIODevice*,quint16)), this, SLOT(newConnectionPassedHelloPhase(QIODevice*,quint16)));
}
void MultiServerBusiness::startAll3Listening()
{
    m_Helloer.startAll3Listening();
}
void MultiServerBusiness::newConnectionPassedHelloPhase(QIODevice *theConnection, quint16 clientId)
{
    m_ActiveConnectionIdsByIODevice.insert(theConnection, clientId);

    //disconnect anyone who is listening to readyRead (the helloer in this case)
    disconnect(theConnection, SIGNAL(readyRead()), 0, 0);

    //connect it to ourselves
    connect(theConnection, SIGNAL(readyRead()), this, SLOT(clientSentUsData()));


    //TODOreq: remove old connections with this clientId here (perhaps before inserting the new connection)
}
void MultiServerBusiness::clientSentUsData()
{
    QIODevice *client = static_cast<QIODevice*>(sender());

    if(client)
    {
        quint16 clientId = m_ActiveConnectionIdsByIODevice.value(client, 0);
        if(clientId > 0)
        {
            //read the message into a recycled container or whatever the fuck
            //set it's clientId
            //process it


            //later (not in here):
            //deliver() received, send to multi server using clientId
        }
        else
        {
            //client doesn't have an ID? Somehow got out of helloer or something without one? should never happen
        }
    }
    else
    {
        //cast failed wtf invalid sender()? should never happen
    }
}
