#ifndef IRPCSERVERIMPL_H
#define IRPCSERVERIMPL_H

#include <QObject>
#include <QHash>

#include "messageDispensers/broadcastdispensers.h"

class CreateBankAccountMessage;

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
signals:
    void d(const QString &);
};

#endif // IRPCSERVERIMPL_H
