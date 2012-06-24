#ifndef CREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CREATEBANKACCOUNTMESSAGEDISPENSER_H

#include <QObject>


//TODOreq: in the interest of not connecting/disconnecting over and over, recycled messages will keep their connections. seeing as they are per-message-type, this works out nicely. only when getting a new one do we need to make it's connection, and only when deleting the messages (program shutdown) do we disconnect them

class CreateBankAccountMessageDispenser : public QObject
{
    Q_OBJECT
public:
    QString *username() { return m_BackingNetworkMessage->string0(); }

    //the responseReady() signal is inheritied
    //as is the doneWithMessage() signal

    //TODOreq: i think, for the broadcasts (pending balance detected for example), i'm going to have to do QObject::moveToThread to make it so the dispenser lives on the bitcoin thread. my ::get is ALWAYS on the same thread... it is only the ::recycle that can occur accross threads (but Qt::AutoConnection is the determiner of that). i am now making it a requirement that ::gets happen on a QObject that lives in the same thread in which you are calling it. perhaps Mutexes are the way instead?
    //perhaps, as a safety requirement, i can do like PendingBalanceDetectedMessageDispenser::takeOwnerShip(&bitcoinHelper); <-- bitcoinHelper would need to be on it's own thread before i make this call. so it'd be in an init or something
    //and like i could make it so you can't use it until ownership is declared. this is an additional codepath that might not be worth it. a boolean check at the beginning of every ::get.... JUST for this functionality? fuck that. perhaps i can make it not work in some other way though... like have ::get just emit a signal (Qt::DirectConnection)... and what the signal is connected to (whether or not it returns a message) is based solely on if I have done takeOwnership. constructor = connects it to .... nothing? or some slot that just emits an error. takeOwnership connects it to the actual ::get. both use direct connections.
    //is a jump faster than a boolean check?
    //methinks NO
    //jumps are slow as fuck, so unless i inline it, i should do the boolean check
    //man talk about premature ejaculation...
    //rofl
    //i love 'theory' <3
    //time to figure out if i can do inline slots :-P

    //just google'd 'qt inline slot'
    //then common sense kicked in rofl
    //how can you inline a signal... which BY DEFINITION is decoupled from the slot
    //if you were even able to, how could you connect multiple slots?
    //i guess you could just call the slots one by one (inline'd)
    //so MAYBE possible
    //but i doubt it would be easy
};

#endif // CREATEBANKACCOUNTMESSAGEDISPENSER_H
