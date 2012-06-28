#ifndef BANKSERVERACTIONMESSAGEDISPENSER_H
#define BANKSERVERACTIONMESSAGEDISPENSER_H

#include <QObject>

//so like wtf? this is a message dispenser dispenser?
//yea pointless.
//the messages should share a dispenser
//perhaps with the broadcasts (though maybe not, since there are complications)

class BankServerActionsMessageDispensers : public QObject
{
    Q_OBJECT
public:
    BankServerActionsMessageDispensers(QObject *parent = 0)
        : QObject(parent)
    {
        m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser();
    }

    //they each get there own dispenser... but the dispensers are all the same (or are they? how will they know what type to 'new' ? perhaps each message implments an IDispensableMessage that has a single method: IDispensableMessage *getNewOfType()=0; ... each message overwrites it and returns a new one of himself. this would mean we'd need to cast it back into the right type later... fml. maybe isn't such a bad thing though?)
    //all i know is this: the ::get and ::recycle shit can DEFINITELY be re-used. fuck tbh i forget if I'm trying to i I i I i i iI iI forget if I'm trying to implement the recycling or the 'rigging' (messageReadyForClient) part................ not that it matters too much, I think in both cases an interface is the answer (the same interface though? that's the question. perhaps NOT, only because rpc client has no use for 'messageReadyForClient'. it has no message 'rigging'... only server does. cool thought: i never have to deal with this ever again. i can just use the rpc generator for it :-P)
    //GOT IT: the dispenser is a template :-). fuck casting.
    IActionMessageDispenser *createBankAccountMessageDispenser() { return m_CreateBankAccountMessageDispenser; }
    //CreateBankAccountMessageDispenser *createBankAccountMessageDispenser() { return m_CreateBankAccountMessageDispenser; }
private:
    IActionMessageDispenser *m_CreateBankAccountMessageDispenser;
};

#endif // BANKSERVERACTIONMESSAGEDISPENSER_H
