#ifndef CREATEBANKACCOUNTREQUEST_H
#define CREATEBANKACCOUNTREQUEST_H

#include <QList>

struct CreateBankAccountRequest
{
    enum FailedReasons
    {
        //TODOreq: these will be defined in the initial xml as properties of the CreateBankAccount Request... err whichever ISN'T the broadcast... oh i called it an Action how clever
        UsernameAlreadyExists,
        PersistError //db error etc
    };
    CreateBankAccountRequest(uint clientId)
    {
        m_ClientId = clientId;
    }

    uint m_ClientId;
    QList<CreateBankAccountRequest::FailedReasons> m_FailedReasons;

    //actual members
    QString m_Username;

    void setFailedAccountAlreadyExists() { m_FailedReasons.append(CreateBankAccountRequest::UsernameAlreadyExists); }
    void setFailedPersistError() { m_FailedReasons.append(CreateBankAccountRequest::PersistError); }
    bool hasErrors() { if(m_FailedReasons.count() > 0) { return true; } return false; }
};

#endif // CREATEBANKACCOUNTREQUEST_H
