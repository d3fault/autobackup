#include "eventemittingplaintextedit.h"

#include <QKeySequence>

#include "../../timelinenodetypes/keypressinnewemptydoctimelinenode.h"
#include "../../iosioscopycatclient.h"

EventEmittingPlainTextEdit::EventEmittingPlainTextEdit(IOsiosClient *osiosClient, IOsiosCopycatClient *copycatClient, QWidget *parent)
    : QPlainTextEdit(parent)
{
    connect(this, SIGNAL(actionOccurred(TimelineNode)), osiosClient->asQObject(), SIGNAL(actionOccurred(TimelineNode)));

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
    TimelineNode keyPressTimelineNode = new KeyPressInNewEmptyDocTimelineNode(e->key());
    emit actionOccurred(keyPressTimelineNode);
    QPlainTextEdit::keyPressEvent(e);
}
void EventEmittingPlainTextEdit::connectToOsiosClientsCopyCatSignals(IOsiosCopycatClient *osiosCopycatClient)
{
    connect(osiosCopycatClient->asQObject(), SIGNAL(synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode*)), this, SLOT(synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode*)));
}
void EventEmittingPlainTextEdit::disconnectToOsiosClientsCopyCatSignals(IOsiosCopycatClient *osiosCopycatClient)
{
    disconnect(osiosCopycatClient->asQObject(), SIGNAL(synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode*)), this, SLOT(synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode*)));
}
//KEY PRESS EVENT -- SYNTHESIZE
void EventEmittingPlainTextEdit::synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode *keyPressInNewEmptyDocTimelineNodeTimelineNode)
{
    QTextCursor myCursor = textCursor();
    myCursor.insertText(QKeySequence(keyPressInNewEmptyDocTimelineNodeTimelineNode->KeyPressed).toString());
    setTextCursor(myCursor);
}
