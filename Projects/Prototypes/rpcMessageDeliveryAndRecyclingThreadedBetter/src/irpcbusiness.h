#ifndef IRPCBUSINESS_H
#define IRPCBUSINESS_H

#include <QObject>

#include "irpcbusinesscontroller.h"

#include "messageDispensers/broadcastdispensers.h"

class IRpcBusiness : public QObject
{
    Q_OBJECT
public:
    explicit IRpcBusiness(QObject *parent = 0);
    BroadcastDispensers *broadcastDispensers();
    void organizeBackendBusinessObjectsThreadsAndStartThem(IRpcBusinessController *clientsHelper);
protected:
    virtual void setParentBackendBusinessObjectForEveryBroadcastDispenser(IRpcBusinessController *clientsHelper)=0;
    virtual void moveBackendBusinessObjectsToTheirOwnThreadsAndStartThem()=0;
    BroadcastDispensers *m_BroadcastDispensers;
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;
signals:
    void initialized();
    void started();
    void stopped(); //gui could listen for this before closing, but it isn't used in the daisy chaining, whereas initialized and started are (we reverse the order on shutdown/stop)
};

#endif // IRPCBUSINESS_H
