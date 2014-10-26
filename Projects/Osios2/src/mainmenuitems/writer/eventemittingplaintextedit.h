#ifndef EVENTEMITTINGPLAINTEXTEDIT_H
#define EVENTEMITTINGPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include "../../iosiosclient.h"

class EventEmittingPlainTextEdit : public QPlainTextEdit, public IOsiosClient /* relay client only */
{
    Q_OBJECT
public:
    explicit EventEmittingPlainTextEdit(IOsiosClient *osiosClient, QWidget *parent = 0);

    QObject *asQObject();
protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void actionOccurred(const ITimelineNode &action);
};

#endif // EVENTEMITTINGPLAINTEXTEDIT_H
