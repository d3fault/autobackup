#ifndef GETTODAYSADSLOTWRESOURCE_H
#define GETTODAYSADSLOTWRESOURCE_H

#include <Wt/WResource>

#include <string>
#include <boost/thread/mutex.hpp>

#include "synchronoustodaysadslotgetter.h"

using namespace Wt;
using namespace std;
using namespace boost;

class GetTodaysAdSlotWResource : public WResource
{
public:
    GetTodaysAdSlotWResource(WObject* parent = 0);
    ~GetTodaysAdSlotWResource();
    bool initialize();
    void handleRequest(const Http::Request& request, Http::Response& response);
private:
    SynchronousTodaysAdSlotGetter m_SynchronousTodaysAdSlotGetter;

    void getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime);
    mutex m_Mutex;
    string m_TodaysAdSlotJson;
    //long long m_TodaysAdSlotExpirationDate;
    int m_NoAdSlotJsonLength;
};

#endif // GETTODAYSADSLOTWRESOURCE_H
