#ifndef SIGNALEMITTINGONKEYEVENTSPLAINTEXTEDIT_H
#define SIGNALEMITTINGONKEYEVENTSPLAINTEXTEDIT_H

#include <QPlainTextEdit>

class QKeyEvent;

class SignalEmittingOnKeyEventsPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit SignalEmittingOnKeyEventsPlainTextEdit(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *keyEvent);
signals:
    keyPressed(qint64 timestamp, int key);
};

#endif // SIGNALEMITTINGONKEYEVENTSPLAINTEXTEDIT_H
