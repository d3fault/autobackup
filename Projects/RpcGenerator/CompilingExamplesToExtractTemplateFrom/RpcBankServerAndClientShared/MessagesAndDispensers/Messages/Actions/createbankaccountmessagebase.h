#ifndef CREATEBANKACCOUNTMESSAGEBASE_H
#define CREATEBANKACCOUNTMESSAGEBASE_H

#include <QString>

class CreateBankAccountMessageBase
{
public:
    bool Success; //keeping this here so my stream operator doesn't break, will deal with it later TODOreq. i don't need or want it though... but have nothing else to return lmao (code needs to be ok with that)
    QString Username;
};

#endif // CREATEBANKACCOUNTMESSAGEBASE_H
