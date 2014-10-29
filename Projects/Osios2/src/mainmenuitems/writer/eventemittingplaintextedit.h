#ifndef EVENTEMITTINGPLAINTEXTEDIT_H
#define EVENTEMITTINGPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include "../../iosiosclient.h"

class IOsiosCopycatClient;
class KeyPressInNewEmptyDocTimelineNode;

class EventEmittingPlainTextEdit : public QPlainTextEdit, public IOsiosClient /* relay client only */
{
    Q_OBJECT
public:
    explicit EventEmittingPlainTextEdit(IOsiosClient *osiosClient, IOsiosCopycatClient *copycatClient, QWidget *parent = 0);

    QObject *asQObject();
protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void actionOccurred(TimelineNode action);
private slots:
    //copycat enable/disable
    void connectToOsiosClientsCopyCatSignals(IOsiosCopycatClient *osiosCopycatClient);
    void disconnectToOsiosClientsCopyCatSignals(IOsiosCopycatClient *osiosCopycatClient);
    //copycat relevant events
    void synthesizeKeyPressedInNewEmptyDocRequested(KeyPressInNewEmptyDocTimelineNode *keyPressInNewEmptyDocTimelineNodeTimelineNode);
};

#endif // EVENTEMITTINGPLAINTEXTEDIT_H
