#include "irecycleabledispenser.h"

#include <QCryptographicHash>

#ifndef RPC_BANK_SERVER_CLIENT_CODE
#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#endif

IRecycleableDispenser::IRecycleableDispenser(QObject *destination, QObject *parent)
    : QObject(parent), m_RecycleableMessagesDispensedCount(0), m_Destination(destination)
{ }
IRecycleableAndStreamable *IRecycleableDispenser::privateGetNewOrRecycled()
{
    if(!m_RecycleList.isEmpty())
    {
        return m_RecycleList.takeLast();
    }
    IRecycleableAndStreamable *newMessage = newOfTypeAndConnectToDestinationObjectIfApplicable();

#ifdef WE_ARE_RPC_BANK_SERVER_CLIENT_CODE //server uses client-set messageId and so doesn't need to set it ever (well, except when reading it off the network, it sets it to whatever it reads off the network from the client)... so it does not need to generate a message id when dispensing (wasted cycles). TODOreq: make sure I actually do that in the network code haha... I've rewritten it to many times to remember...
    QByteArray messageIdArray;
    //TODOreq (pretty sure i don't need this anymore): RPC_BANK_SERVER_UNIQUE_CLIENT_ID needs to be set. it would be optimal to have the Rpc Generator itself set it... and i specify how many clients i want etc. this would go hand in hand with my Rpc Generator setting up all the Ssl Certs etc too. i'd love to be able to say "Generate 10 clients  and 1 server" and it does all the magic for me. which fuck leads me to another question: mutliple servers (sharing same db). need to write about this in it's own doc
    messageIdArray.append(QString(RPC_BANK_SERVER_UNIQUE_CLIENT_ID) + QString::number(m_RecycleableMessagesDispensedCount) + QDateTime::currentDateTime().toString() + QString::number(qrand()));
    //TODOreq (old? probably still relevant tho lol who cares ez): QByteArray to uint conversion? my instincs say to change the unit to a QString or QByteArray, but then allocating the header on the stack isn't as cheap because we'll be using a d_ptr for messageId. this might be an ok conversion though. a bit sloppy, but might work regardless. idfk.
    newMessage->Header.MessageId = QCryptographicHash::hash(messageIdArray, QCryptographicHash::Md5).toHex();
#endif

    connect(newMessage, SIGNAL(doneWithMessageSignal()), this, SLOT(handleMessageReportingItselfDone()));
    ++m_RecycleableMessagesDispensedCount;
    return newMessage;
}
void IRecycleableDispenser::handleMessageReportingItselfDone()
{
    m_RecycleList.append(static_cast<IRecycleableAndStreamable*>(sender()));
}
