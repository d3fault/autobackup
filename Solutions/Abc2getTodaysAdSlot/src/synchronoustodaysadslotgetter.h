#ifndef SYNCHRONOUSTODAYSADSLOTGETTER_H
#define SYNCHRONOUSTODAYSADSLOTGETTER_H

#include "isynchronouslibcouchbaseuser.h"

#include <iostream>
using namespace std;

#include "abc2couchbaseandjsonkeydefines.h"

class SynchronousTodaysAdSlotGetter : public ISynchronousLibCouchbaseUser
{
public:
    SynchronousTodaysAdSlotGetter();
    bool tryToGetTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime);
    string todaysAdSlotJson();
    long long todaysAdSlotExpirationDate();
private:
    void errorOutput(const string &errorString);
    string m_TodaysAdSlotJson;
    long long m_TodaysAdSlotExpirationDate;
};

#endif // SYNCHRONOUSTODAYSADSLOTGETTER_H
