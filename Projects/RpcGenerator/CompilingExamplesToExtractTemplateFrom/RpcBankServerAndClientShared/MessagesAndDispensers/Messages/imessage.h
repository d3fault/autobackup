#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <QObject>
#include <QDataStream>

#include "irecycleableandstreamable.h"

//OUTDATED: for lack of a better place, here's what a message looks like (this is just the overall design, the impl is a lot harder ofc):
/*
struct Message
[
    quint32 MagicAndSYNC, //we read it in by the smallest unit possible at a time (char?) until we've seen all of magic, then we know a header follows
    struct MessageHeader
    [
        quint16 MessageSize,
        quint16 RpcServiceId,
        quint32 MessageId,
        quint16 MessageType
    ],
    QByteArray MessageData
    [
        //re: below two. for streaming a message out, we surround dat code with #ifdef WE_ARE_RPC_BANK_SERVER. for streaming in, we surround it with #ifdef WE_ARE_RPC_BANK_SERVER_CLIENT
        bool actionsOnlySuccess,
        quint16 ifNotSuccessErrorCode,
        THE_REST_OF_THESE_BYTES_ARE_THE_MESSAGE_PARAMETERS_TO_BE_STREAMED_BY_MESSAGE_SPECIFIC_STREAMERS
    ]
]
*/

//solved:
//broadcasts on both sides don't need Success or ErrorCode
//FALSE: actions on client don't need Success or ErrorCode. <<---- ACTUALLY that's false. they don't need them during the request phase, but since we recycle the request for use with the response, they should be there. they just won't be used until later... but they're already allocated
//^^^^ HOWEVER, one thing that's true about it is that the rpc client does not need to stream the success or errorcode values to the server, pointless waste of bandwidth
//^^^^ tie'ing into that, it might be an adequate solution to just have broadcasts not be streaming the values. that way they only waste space in memory (cheap), not bandwidth ('spensive). still, my optimizer brain is all leik "NOOOOOO DONT WASTE MEMORY YO!" :-P
//^^^^^^^^^^^^^^^^^SOLUTION: put the two members in question in the array (the header is shared by every type, and we DON'T want to split it up and stream it piece by piece, which would be required if those two members were a part of the header (not 100% about that fact. it came to me and then i lost it while writing it). there's also the problem of broadcasts NEVER streaming the two in or out, which is yet another reason to not put them in the header. i'm going to have all Actions implement IActionMessage, which will contain the code for streaming those two specific values (and certain parts #ifdef'd out depending on if we're server or client code

class IMessage : public IRecycleableAndStreamable
{
    Q_OBJECT
public:
    explicit IMessage(QObject *owner);
    void deliver();
signals:
    void deliverSignal();
};

#endif // IMESSAGE_H
