#include "eventemittingplaintextedit.h"

#include <QKeySequence>

#include "../../timelinenodetypes/keypressinnewemptydoctimelinenode.h"
#include "../../iosioscopycatclient.h"

//TODOreq: pasting doesn't work, since it isn't a key press. maybe I should use the text changed signals instead, but bleh~ My main reason for not wanting to is that now I'm responsible for remembering the old buffer and then diffing etc.... but hey it's doable and also makes handling meta keys easier (since I only record what they ended up doing, not their actual presses). Pasting is a "multi keystroke", so it can/SHOULD send all the "keystrokes" in one network message (kind of like a "bulk send")
EventEmittingPlainTextEdit::EventEmittingPlainTextEdit(IOsiosClient *osiosClient, IOsiosCopycatClient *copycatClient, QWidget *parent)
    : QPlainTextEdit(parent)
{
    connect(this, SIGNAL(actionOccurred(TimelineNode,QByteArray)), osiosClient->asQObject(), SIGNAL(actionOccurred(TimelineNode,QByteArray)));

    //copycat enabling/disabling
    connect(copycatClient->asQObject(), SIGNAL(connectEventSynthesisSignalsForCopycattingRequested(IOsiosCopycatClient*)), this, SLOT(connectToOsiosClientsCopyCatSignals(IOsiosCopycatClient*)));
    connect(copycatClient->asQObject(), SIGNAL(disconnectEventSynthesisSignalsForCopycattingRequested(IOsiosCopycatClient*)), this, SLOT(disconnectToOsiosClientsCopyCatSignals(IOsiosCopycatClient*)));
}
QObject *EventEmittingPlainTextEdit::asQObject()
{
    return this;
}
//KEY PRESS EVENT -- RECORD
void EventEmittingPlainTextEdit::keyPressEvent(QKeyEvent *e) //TODOreq: differentiate key press and key release? i really want the combination of the two tbh, and an exact amount of repeats is i think alredy sent? So the timeline node stream will surely match the buffer contents of myself (a plain text edit). i worry that key presses and holds won't work properly. they'll work in the gui but not in the serialized data (test this)
{
    //pass through event
    QPlainTextEdit::keyPressEvent(e); //TODOreq: it is mandatory for us to call the base implementation for all of our "recordings" _FIRST_. We don't want to take a screenshot of the pre-event gui state!

    //now record event
    TimelineNode keyPressTimelineNode = new KeyPressInNewEmptyDocTimelineNode(e->key());
    QByteArray plainTextEditScreenshotHash = IOsiosClient::calculateCryptographicHashOfWidgetRenderedToImage(this); //TODOreq: this screenshot contains the cursor (the little blinky line thing that tells you where you're typing), whereas the synthesized screenshot does NOT... so the cryptographic verification is failing :-/. Maybe if I use the textChanged() signal instead it won't anymore?

    emit actionOccurred(keyPressTimelineNode, plainTextEditScreenshotHash);
}
void EventEmittingPlainTextEdit::connectToOsiosClientsCopyCatSignals(IOsiosCopycatClient *osiosCopycatClient)
{
    connect(osiosCopycatClient->asQObject(), SIGNAL(synthesizeKeyPressedInNewEmptyDocRequested(OsiosDhtPeer*,KeyPressInNewEmptyDocTimelineNode*)), this, SLOT(synthesizeKeyPressedInNewEmptyDocRequested(OsiosDhtPeer*,KeyPressInNewEmptyDocTimelineNode*)));
    connect(this, SIGNAL(timelineNodeAppliedAndRendered(OsiosDhtPeer*,TimelineNode,QByteArray)), osiosCopycatClient->asQObject(), SIGNAL(timelineNodeAppliedAndRendered(OsiosDhtPeer*,TimelineNode,QByteArray)));
}
void EventEmittingPlainTextEdit::disconnectToOsiosClientsCopyCatSignals(IOsiosCopycatClient *osiosCopycatClient)
{
    disconnect(osiosCopycatClient->asQObject(), SIGNAL(synthesizeKeyPressedInNewEmptyDocRequested(OsiosDhtPeer*,KeyPressInNewEmptyDocTimelineNode*)), this, SLOT(synthesizeKeyPressedInNewEmptyDocRequested(OsiosDhtPeer*,KeyPressInNewEmptyDocTimelineNode*)));
    disconnect(this, SIGNAL(timelineNodeAppliedAndRendered(OsiosDhtPeer*,TimelineNode,QByteArray)), osiosCopycatClient->asQObject(), SIGNAL(timelineNodeAppliedAndRendered(OsiosDhtPeer*,TimelineNode,QByteArray)));
}
//KEY PRESS EVENT -- SYNTHESIZE
void EventEmittingPlainTextEdit::synthesizeKeyPressedInNewEmptyDocRequested(OsiosDhtPeer *osiosDhtPeer, KeyPressInNewEmptyDocTimelineNode *keyPressInNewEmptyDocTimelineNodeTimelineNode)
{
    //apply event -- ODOreq: it is mandatory for us to our "event synthesis" _FIRST_. We don't want to take a screenshot of the pre-event gui state!
    QTextCursor myCursor = textCursor();
    myCursor.insertText(QKeySequence(keyPressInNewEmptyDocTimelineNodeTimelineNode->KeyPressed).toString());
    setTextCursor(myCursor);

    //render for proofz
    QByteArray plainTextEditScreenshotHash = IOsiosClient::calculateCryptographicHashOfWidgetRenderedToImage(this);

    emit timelineNodeAppliedAndRendered(osiosDhtPeer, keyPressInNewEmptyDocTimelineNodeTimelineNode, plainTextEditScreenshotHash);
}
