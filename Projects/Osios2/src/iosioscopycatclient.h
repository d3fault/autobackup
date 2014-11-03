#ifndef IOSIOSCOPYCATCLIENT_H
#define IOSIOSCOPYCATCLIENT_H

class QObject;

class OsiosDhtPeer;
class KeyPressInNewEmptyDocTimelineNode;

//being a copycat client means you are re-synthesizing events (kb/mouse/etc) that a client generated, probably on the other side of a network connection
class IOsiosCopycatClient //TODOreq: merge into IOsiosClient, since copycatting/rendering/hasing/snedding-hash-with-response is core part of app functionality
{
public:
    virtual void setCopycatModeEnabled(bool enabled)=0;
    virtual QObject *asQObject()=0;
protected: //signals
    virtual void connectEventSynthesisSignalsForCopycattingRequested(IOsiosCopycatClient *osiosCopycatClientToListenToSignalsFrom)=0;
    virtual void disconnectEventSynthesisSignalsForCopycattingRequested(IOsiosCopycatClient *osiosCopycatClientToStopListeningToSIgnalsFrom)=0;

    virtual void synthesizeKeyPressedInNewEmptyDocRequested(OsiosDhtPeer *osiosDhtPeer, KeyPressInNewEmptyDocTimelineNode *keyPressInNewEmptyDocTimelineNodeTimelineNode)=0;
};

#endif // IOSIOSCOPYCATCLIENT_H
