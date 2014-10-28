#include "eventemittingplaintextedit.h"

#include "../../timelinenodetypes/keypressinnewemptydoctimelinenode.h"

EventEmittingPlainTextEdit::EventEmittingPlainTextEdit(IOsiosClient *osiosClient, QWidget *parent)
    : QPlainTextEdit(parent)
{
    connect(this, SIGNAL(actionOccurred(TimelineNode)), osiosClient->asQObject(), SIGNAL(actionOccurred(TimelineNode)));
}
QObject *EventEmittingPlainTextEdit::asQObject()
{
    return this;
}
void EventEmittingPlainTextEdit::keyPressEvent(QKeyEvent *e) //TODOreq: differentiate key press and key release? i really want the combination of the two tbh, and an exact amount of repeats is i think alredy sent? So the timeline node stream will surely match the buffer contents of myself (a plain text edit). i worry that key presses and holds won't work properly. they'll work in the gui but not in the serialized data (test this)
{
    TimelineNode keyPressTimelineNode = new KeyPressInNewEmptyDocTimelineNode(e->key());
    emit actionOccurred(keyPressTimelineNode);
    QPlainTextEdit::keyPressEvent(e);
}
