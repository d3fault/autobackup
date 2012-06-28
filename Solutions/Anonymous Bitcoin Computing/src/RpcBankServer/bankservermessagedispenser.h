#ifndef BANKSERVERMESSAGEDISPENSER_H
#define BANKSERVERMESSAGEDISPENSER_H

#include <QObject>

#include "Messages/bankserveractionmessagedispenser.h"
#include "Messages/bankserverbroadcastmessagedispenser.h"

class BankServerMessageDispenser : public QObject
{
    Q_OBJECT
public:
    explicit BankServerMessageDispenser(QObject *parent = 0)
        : QObject(parent)
    {
        m_BankServerActionMessageDispenser = new BankServerActionsMessageDispensers();
        m_BankServerBroadcastMessageDispenser = new BankServerBroadcastsMessageDispensers();
    }
    BankServerMessageDispenser::~BankServerMessageDispenser()
    {
        //TODOreq: clean up. perhaps first ensure all messages are back? this might need to be in a .cleanup() or by marking something as done or something

        //delete m_BankServerBroadcastMessageDispensers;
        //delete m_BankServerActionMessageDispensers;
    }
    BankServerActionsMessageDispensers *actions() { return m_BankServerActionMessageDispenser; }
    BankServerBroadcastsMessageDispensers *broadcasts() { return m_BankServerBroadcastMessageDispenser; }
private:
    BankServerActionsMessageDispensers *m_BankServerActionMessageDispenser;
    BankServerBroadcastsMessageDispensers *m_BankServerBroadcastMessageDispenser;
signals:

public slots:

};

#endif // BANKSERVERMESSAGEDISPENSER_H
