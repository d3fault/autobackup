#ifndef BANKSERVERACTIONMESSAGEDISPENSER_H
#define BANKSERVERACTIONMESSAGEDISPENSER_H

#include <QObject>

//so like wtf? this is a message dispenser dispenser?
//yea pointless.
//the messages should share a dispenser
//perhaps with the broadcasts (though maybe not, since there are complications)

class BankServerActionMessageDispenser : public QObject
{
    Q_OBJECT
public:
    BankServerActionMessageDispenser(QObject *parent = 0)
        : QObject(parent)
    {
        m_CreateBankAccountMessageDispenser = new CreateBankAccountMessageDispenser();
    }

    CreateBankAccountMessageDispenser *createBankAccountMessageDispenser() { return m_CreateBankAccountMessageDispenser; }
private:
    CreateBankAccountMessageDispenser *m_CreateBankAccountMessageDispenser;
};

#endif // BANKSERVERACTIONMESSAGEDISPENSER_H
