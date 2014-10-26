#include "eventemittingplaintextedit.h"

#include "../../timelinenodetypes/keypressinnewemptydoctimelinenode.h"

EventEmittingPlainTextEdit::EventEmittingPlainTextEdit(IOsiosClient *osiosClient, QWidget *parent)
    : QPlainTextEdit(parent)
{
    connect(this, SIGNAL(actionOccurred(ITimelineNode)), osiosClient->asQObject(), SIGNAL(actionOccurred(ITimelineNode)));
}
QObject *EventEmittingPlainTextEdit::asQObject()
{
    return this;
}
void EventEmittingPlainTextEdit::keyPressEvent(QKeyEvent *e) //TODOreq: differentiate key press and key release? i really want the combination of the two tbh. for now doing it like this means key presses and holds won't work properly. they'll work in the gui but not in the serialized data
{
    KeyPressInNewEmptyDocTimelineNode keyPressTimelineNode(e->key());
    emit actionOccurred(keyPressTimelineNode);
    QPlainTextEdit::keyPressEvent(e);
}
