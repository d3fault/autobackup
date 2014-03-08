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
    void getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime);
private:
    void errorOutput(const string &errorString);
};

#endif // SYNCHRONOUSTODAYSADSLOTGETTER_H
