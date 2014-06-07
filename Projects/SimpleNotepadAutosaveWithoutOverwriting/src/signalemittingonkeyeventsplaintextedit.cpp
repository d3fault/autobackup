#include "signalemittingonkeyeventsplaintextedit.h"

#include <QDateTime>
#include <QKeyEvent>

SignalEmittingOnKeyEventsPlainTextEdit::SignalEmittingOnKeyEventsPlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{ }
void SignalEmittingOnKeyEventsPlainTextEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    emit keyPressed(QDateTime::currentMSecsSinceEpoch(), keyEvent->key());
}
