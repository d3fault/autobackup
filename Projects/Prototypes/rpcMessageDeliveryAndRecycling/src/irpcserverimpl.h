#ifndef IRPCSERVERIMPL_H
#define IRPCSERVERIMPL_H

#include <QObject>
#include <QHash>

class IRpcServerImpl : public QObject
{
    Q_OBJECT
public:
    QHash<BroadcastEnum, BroadcastDispenser*> *broadcastDispensers() { return m_BroadcastDispensers; }

private:
    QHash<BroadcastEnum, BroadcastDispenser*> *m_BroadcastDispensers;

public slots:
    virtual void createBankAccount(CreateBankAccountMessage *createBankAccountMessage)=0;
};

#endif // IRPCSERVERIMPL_H
