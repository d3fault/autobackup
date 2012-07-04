#ifndef ACTIONDISPENSERS_H
#define ACTIONDISPENSERS_H

#include <QObject>

#include "../irpcclientshelper.h"
#include "actions/createbankaccountmessagedispenser.h"

class ActionDispensers : public QObject
{
    Q_OBJECT
public:
    explicit ActionDispensers(IRpcClientsHelper *destinationObject)
    {
        m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser(destinationObject);
    }

    CreateBankAccountMessageDispenser *createBankAccountMessageDispenser() { return m_CreateBankAccountMessageDispenser; }
private:
    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
};

#endif // ACTIONDISPENSERS_H
