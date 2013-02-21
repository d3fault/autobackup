#ifndef NETWORKMAGIC_H
#define NETWORKMAGIC_H

#include <QObject>
#include <QDataStream>

#define MAGIC_BYTE_SIZE 4 //if you change this, you also need to change the m_MagicExpected array to match and also the 'stream magic' helper function


//TODOoptimization: that same optimization about keeping clientId as a member right next to a slot (connections have their own OBJECTS) also applies to Magic. In fact I'm considering making a "ConnectionDetails" object that hasA both HelloStatus and Magic... so that ConnectionDetails basically becomes the object which holds the slot that I connect each connection to
//TODOreq: It would mean that two QIODevices with the same clientId would use the same object (unless we ensure that there are never two 'connected' (both network-y and qobject-y) simultaneously. we need to cleanly handle that disconnect/reconnect case)
//re: ^^^^ i guess what it really means is that i store the objects that contain slots in a hash and only look them up once during the init/connect phase.... from then on they use their own members blindly. it's very similar to how i do it now except it moves the hash lookup to a much more efficient place. oh and we also now only do it once :). signals/slots _ALWAYS_ has to determine who the destination object is NO MATTER WHAT... so that lookup remains the same (especially since we aren't doing anything across threads)
//TODOreq: when do we delete the connection-specific objects? and TODOreq more importantly: the businesses would be now emitting to the connection-specific object? how the fuck does that work? how the fuck does that work without me disconnect/reconnecting all the damn time. wtf is faster, a disconnect/reconnect or a hash lookup? but the point of this TODOreq is mainly to ensure that the backend objects don't emit to a null object. we should catch said messages (whether broadcasts or action responses, idfk which (but most likely action responses i guess? broadcasts would just choose another alive object???)) in a queue thingy or some shit IDFK my brain hurts
// ^^^^ TODOreq: so perhaps the object-per-connection idea backfires in the long run. i need to do some crazy thinking/examining. perhaps even testing QObject::disconnect/reconnect vs. hash lookups (by clientId i guess?). OR it would really just mean that each connection-object maintains IT'S OWN "message dispenser" that is rigged for him specifically??????????? mind = blown. this way we don't have to do disconnect/reconnect, OR hash lookup. signals/slots only do their "who is destination object", which it DOES NO MATTER WHAT
//TODOreq: the 'multiple [stale] connections use same object-slot' issue is there no matter what, be aware of it

//why does the message need a clientId then? couldn't the object (the one that owns the slot that the specific connection's readyRead is connected to) just contain the QIODevice as a member????? my brain hurts right now. MAKE IT FUNCTIONAL FUCKER!!!!!!! premature optimization.
//the qiodevice member is simply... overwritten (TODOreq: don't overwrite it when we have half-read or half-written a message) when a new connection is detected. so we still need cookies etc to detect a reconnect, just not a clientId attached to each message [in memory]?

//BASICALLY, and i _THINK_ this holds true: deliver() doesn't need to give a shit which connection-object the message is delivered to. we connect the objects smartly to handle it all for us.
//HOWEVER, broadcasts still need to go to a 'global connection-object' (one that sees all connections) in order to function. mass redesign required sounds like.... and fuuuuuck i'm already in the middle of a mass redesign so FUCK YOU [for now]
struct NetworkMagic
{
    NetworkMagic(QIODevice *ioDeviceToLookForMagicOn);
    void setIoDeviceToLookForMagicOn(QIODevice *ioDeviceToLookForMagicOn);
    static const quint8 MagicExpected[MAGIC_BYTE_SIZE];
    quint8 m_CurrentMagicByteIndex;
    bool m_MagicGot;

    static inline void streamOutMagic(QDataStream *ds)
    {
        *ds << MagicExpected[0];
        *ds << MagicExpected[1];
        *ds << MagicExpected[2];
        *ds << MagicExpected[3];
    }

    QIODevice *m_IoDeviceToLookForMagicOn;
    QDataStream m_DataStreamToLookForMagicOn;

    bool consumeFromIODeviceByteByByteLookingForMagic_And_ReturnTrueIf__Seen_or_PreviouslySeen__And_FalseIf_RanOutOfDataBeforeSeeingMagic();
    inline void messageHasBeenConsumedSoPlzResetMagic()
    {
        m_MagicGot = false;
        m_CurrentMagicByteIndex = 0;
    }
};

#endif // NETWORKMAGIC_H
