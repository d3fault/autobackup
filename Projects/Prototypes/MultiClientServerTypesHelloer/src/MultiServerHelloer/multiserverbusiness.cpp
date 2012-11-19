#include "multiserverbusiness.h"

MultiServerBusiness::MultiServerBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_ServerHelloer, SIGNAL(connectionHasBeenHelloedAndIsReadyForAction(QIODevice*,quint32)), this, SLOT(newConnectionPassedHelloPhase(QIODevice*,quint32))); //careful coding must ensure that this is a direct connection (but we still shouldn't explicitly make it direct, because that means you coded it wrong (though i _GUESS_ you could take on the responsibility of mutex protecting etc, lol glhf))
    connect(&m_ServerHelloer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
}
void MultiServerBusiness::startAll3Listening()
{
    m_ServerHelloer.startAll3Listening();
}
void MultiServerBusiness::newConnectionPassedHelloPhase(QIODevice *theConnection, quint32 clientId)
{
    m_ActiveConnectionIdsByIODevice.insert(theConnection, clientId);

    //the emitter has already disconnected himself

    //connect it to ourselves
    connect(theConnection, SIGNAL(readyRead()), this, SLOT(clientSentUsData()));


    //TODOreq: remove old connections with this clientId here (perhaps before inserting the new connection)
}
void MultiServerBusiness::clientSentUsData()
{
    QIODevice *client = static_cast<QIODevice*>(sender());

    if(client)
    {
        quint32 clientId = m_ActiveConnectionIdsByIODevice.value(client, 0);
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
