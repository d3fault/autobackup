#ifndef GETTODAYSADSLOTWRESOURCE_H
#define GETTODAYSADSLOTWRESOURCE_H

#include <Wt/WResource>

#include <string>
#include <boost/thread/mutex.hpp>

using namespace Wt;
using namespace std;
using namespace boost;

class GetTodaysAdSlotWResource : public WResource
{
public:
    GetTodaysAdSlotWResource(WObject* parent = 0);
    ~GetTodaysAdSlotWResource();
    void handleRequest(const Http::Request& request, Http::Response& response);
private:
    static void getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex, long long currentDateTime);
    static mutex m_TodaysAdSlotMutex;
    static string m_TodaysAdSlotJson;
    static long long m_TodaysAdSlotExpirationDate;
};

#endif // GETTODAYSADSLOTWRESOURCE_H
