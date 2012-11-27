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


    emit atLeastOneClientIsConnected(true); //TODOreq: on disconnects, see if at least one is still connected and emit false. it's just overly testing though so fuck it...

    //TODOreq: remove old connections with this clientId here (perhaps before inserting the new connection)
}
void MultiServerBusiness::clientSentUsData()
{
    QIODevice *client = static_cast<QIODevice*>(sender());

    if(client) //TODOreq: is this even going to verify anything since it's not a dynamic_cast or qobject_cast ??
    {
        quint32 clientId = m_ActiveConnectionIdsByIODevice.value(client, 0);
        if(clientId > 0)
        {
            MultiServerAbstraction design makes more sense to use here than to copy/paste code the magic finding / message size peeking methods here. protocol knower shouldnt have to deal with an iodevice at all (except there will be one hidden: qbuffer... when we wrap the byte array in a qdatastream in order to decipher the message and handle its protocl implementation. what this means is i think i need a new prototype lmfao. should probably design it first tbh)


            QString theMessageText;

            //read the message into a recycled container or whatever the fuck
            //set it's clientId
            //process it


            //later (not in here):
            //deliver() received, send to multi server using clientId
            //^^^^DBG_sendMessageToClient is pretty much this for this prototype/test
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
void MultiServerBusiness::DBG_sendMessageToClient(const QString &message)
{
    //this is simulating broadcasts (without neighbor notification) i guess... lol fuck it. this is very hacky and only used for this testing

    QByteArray theMessageAsArray;
    QDataStream theMessageStream(&theMessageAsArray, QIODevice::WriteOnly);
    theMessageStream << message;

    QList<quint32> clientIds = m_ActiveConnectionIdsByIODevice.values();
    QListIterator<quint32> it(clientIds);

    while(it.hasNext())
    {
        quint32 currentClientId = it.next();
        m_ServerHelloer.sendMessageArrayToClientId(theMessageAsArray, currentClientId); //TODOreq: so i guess it's more of like the server that's aware of all the connections that is communicated to by Id, it is NOT _ONLY_ a "helloer". A rename is in order... but i havent' come up with anything yet...
    }
}
