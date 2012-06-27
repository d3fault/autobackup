#ifndef CREATEBANKACCOUNTMESSAGEDISPENSER_H
#define CREATEBANKACCOUNTMESSAGEDISPENSER_H

#include <QObject>



//TODOreq: idk if this belongs here per se, but i want to make a note (writing down helps me remember anyways): there is no reason for the business impl to need any of the actions dispensers... only the broadcast dispensers. the actions themselves pass individual messages as their arguments. only the RpcClientsHelper (auto-generated code) needs to deal with the dispensers for Actions

//TODOreq: in the interest of not connecting/disconnecting over and over, recycled messages will keep their connections. seeing as they are per-message-type, this works out nicely. only when getting a new one do we need to make it's connection, and only when deleting the messages (program shutdown) do we disconnect them

//the rpc messages can have different backing network messages each time
//upon 'recycle', they are no longer associated (even though they still are)
//upon 'get' (a recycled), we are forced into associating a new backing network message. the one we just read it (side note: it too was recycled :-P)

class CreateBankAccountMessageDispenser : public QObject
{
    Q_OBJECT
public:
    void setMessageReadyReceiver();

    QString *username() { return m_BackingNetworkMessage->string0(); } //lol wut? this is the dispenser, not the message itself!!!!!!!!!!
    //I suppose I can define the message + the dispenser in the same file... this file... because I don't want 2 files per Action + 2 files per Broadcast ehh

    //does each type need it's OWN dispenser?
    //I should elaborate
    //does each Action/Broadcast message need it's own dispenser TYPE? can't they just share the type, each having their own instance?
    //well... yes... but should I still call them by the message in which they dispense?
    //I'd say yes... but the type should not specify it... the variable/member name should!!!!
    //lol dommit this project's getting semi-messy. not lost yet but still lots of failed messages attempts etc lmao

    //fuck, wat do. was about to write "committing before obliterating / cleaning up"... but there's a lot of TODOreq's in here
    //wouldn't ifdef'ing them out and removing from .pro mean they don't show up in project-wide searching?
    //wat do. maybe so many comments and TODOs don't belong in the source itself
    //but fuck me I can't think of any other place...

    //the responseReady() signal is inheritied
    //as is the doneWithMessage() signal

    //responseReady() doesn't make sense for broadcasts, but something similar certainly does. the wording would just be different. emit broadcast.broadcastReady(); for example. doneWithMessage() is the same~
    //tryin to think if Actions/Broadcasts should inherit from same class
    //maybe an emit rpcMessage.messageReadyForClient(); <-- the base abstract class/interface between Actions/Broadcasts
    //some more design considerations need to take place with regard to message dispensers (doneWithMessage() goes back to the dispenser)... but I think I can get this as I code it... doesn't sound too difficult

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


    //another idea I just had:
    //a 'private' backing class
    //takeOwnership() swaps the backing dummy class
    //the method directly called on the private backing class(es) can be inlined
    //before takeOwnership, backing class does jack shit
    //afterwards, it works
    //that would at least create segfaults :). I'd still need to check the return value isn't zero.

    //oh fuck TODOreq: no recycled messages available, no memory ('new' fails), ... wait for recycled? block? wtf? rare condition anyways, deal with it later
};
void CreateBankAccountMessageDispenser::setMessageReadyReceiver(SomeTypePossiblyServerAndProtocolKnower *receiver)
{
    //this function just sets the receiver for later use when new'ing. it comes in through the parameter. instincts/logic atm say it should be the ServerAndProtocolKnower object... but since we're going to use this on the RpcClient also... that clearly does not make any sense. I'm thinking an interface that ServerAndProtocolKnower implements...

    //THEN AGAIN, the kind of things that we'll be rigging will be different (methinks opposite) on the client... so maybe not?
    //and there is no "messageReadyForClient" signal on the client... nor is there an equivalent "messageReadyForServer" (although there could be)... we just pass the message directly to the exposed method for whatever action we want
    //client still does use the recycling shit. I'm wondering if I need to hit up dia to help my brain :-P

    //on new'ing in ::get, we do this:
    connect(newMessage, SIGNAL(messageReadyForClient(CreateBankAccountMessage*)), receiverPassedInAsParameter, SLOT(createBankAccountCompleted(CreateBankAccountMessage*)));
    //to use it is a bit redundant, but w/e:
    emit messageWeFilledOut.messageReadyForClient(messageWeFilledOut); //weird because we're passing ourselves in a signal that we own. I don't think this is illegal, and there might be a better way to do it. but for now, it works (<- assumption, pretty sure) so fuck it.

    //fuck this, so lost. dia it is

    //obviously this would all be in some interface that CreateBankAccountMessageDispenser implements...
}

#endif // CREATEBANKACCOUNTMESSAGEDISPENSER_H
