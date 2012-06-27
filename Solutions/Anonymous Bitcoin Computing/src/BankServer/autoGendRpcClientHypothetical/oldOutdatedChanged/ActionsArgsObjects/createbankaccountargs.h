#ifndef CREATEBANKACCOUNTREQUEST_H
#define CREATEBANKACCOUNTREQUEST_H

#include <QList>

//i think this should only have members to access and provide abstraction from the network message itself. the failed helpers are still cool, but it should operate on the message itself. we do not have POD in this object (duplicate enums are ok though if it makes sense). this object gets passed around business logic, but the protocol/network-message should be completely hidden from view at that point (despite being the same data. no need to/inefficient to copy it before processing it. just ABSTRACT it)


//now i'm wondering if this ArgObject is relevant on the Rpc Client side of things?

struct CreateBankAccountArgs //args takes care of me wondering if i need request/response. they should both be the same message. fuck it.
{
    enum FailedReasons
    {
        UsernameAlreadyExists,
        PersistError //db error etc
    };
    CreateBankAccountArgs(uint clientId)
    {
        m_ClientId = clientId;
    }

    //uint m_ClientId; abstract this
    //QList<CreateBankAccountRequest::FailedReasons> m_FailedReasons; abstract this

    //actual members
    //QString m_Username;
    const QString &username() { return m_SubMessageAbstracted.m_String0; /* or m_Username, tbd */ } //or maybe the const goes after the word 'username'? idfk. TODOreq: figure this shit out. your C++ skills will improve.


    //make all these use the network message's data
    void setFailedAccountAlreadyExists() { m_FailedReasons.append(CreateBankAccountArgs::UsernameAlreadyExists); }
    void setFailedPersistError() { m_FailedReasons.append(CreateBankAccountArgs::PersistError); }
    bool hasErrors() { if(m_FailedReasons.count() > 0) { return true; } return false; }
};

#endif // CREATEBANKACCOUNTREQUEST_H
