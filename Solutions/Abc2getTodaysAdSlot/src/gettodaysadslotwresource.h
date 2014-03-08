#ifndef GETTODAYSADSLOTWRESOURCE_H
#define GETTODAYSADSLOTWRESOURCE_H

#include <Wt/WResource>

#include <string>

using namespace Wt;
using namespace std;

class GetTodaysAdSlotWResource : public WResource
{
public:
    GetTodaysAdSlotWResource(WObject* parent = 0);
    ~GetTodaysAdSlotWResource();
    void handleRequest(const Http::Request& request, Http::Response& response);
private:
    void getTodaysAdSlot(const string &campaignOwnerUsername, const string &campaignIndex);
};

#endif // GETTODAYSADSLOTWRESOURCE_H
