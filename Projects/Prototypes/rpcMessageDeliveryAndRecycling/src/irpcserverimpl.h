#ifndef IRPCSERVERIMPL_H
#define IRPCSERVERIMPL_H

#include <QObject>
#include <QHash>

#include "broadcastdispensers.h"

class IRpcServerImpl : public QObject
{
    Q_OBJECT
public:
    BroadcastDispensers *broadcastDispensers() { return m_BroadcastDispensers; }
    void setBroadcastDispensers(BroadcastDispensers *broadcastDispensers) { m_BroadcastDispensers = broadcastDispensers; }
private:
    BroadcastDispensers *m_BroadcastDispensers;
public slots:
    virtual void createBankAccount(CreateBankAccountMessage *createBankAccountMessage)=0;
};

#endif // IRPCSERVERIMPL_H
