#ifndef IMESSAGEDISPENSER_H
#define IMESSAGEDISPENSER_H

#include <QObject>

#include "../irpcbankservermessagetransporter.h"
#include "messages/imessage.h"

//TODOoptimization: the best part about these message dispensers as an interface is that it would be trivial to implement "list swapping" where there are 2 lists and we start with them both empty and just have 10 in each at the end of the day. that way, only 1 'cross thread message' needs to occur once every 10 messages. much faster message recycling imo... though technically less efficient. would we ever need 20 messages at low server load? answer: no
//but 10 is also arbitrary...

class IMessageDispenser : public QObject
{
    Q_OBJECT
public:
    explicit IMessageDispenser(QObject *mandatoryParent);
    void setDestinationObject(IRpcBankServerMessageTransporter *destinationObject);
private:
    QList<IMessage*> m_RecycleList;
protected:
    IRpcBankServerMessageTransporter *m_DestinationObject;
    IMessage *privateGetNewOrRecycled();
    virtual IMessage *getNewOfTypeAndConnectToDestinationObject()=0;
public slots:
    void handleMessageReportingItselfDone();
};

#endif // IMESSAGEDISPENSER_H
