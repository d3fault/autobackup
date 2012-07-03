#ifndef ACTIONDISPENSERS_H
#define ACTIONDISPENSERS_H

#include <QObject>

#include "actions/createbankaccountmessagedispenser.h"

class ActionDispensers : public QObject
{
    Q_OBJECT
public:
    CreateBankAccountMessageDispenser *createBankAccountMessageDispenser() { return m_CreateBankAccountMessageDispenser; }
    explicit ActionDispensers(QObject *parent = 0)
        : QObject(parent)
    {
        m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser();
    }
private:
    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
};

#endif // ACTIONDISPENSERS_H
