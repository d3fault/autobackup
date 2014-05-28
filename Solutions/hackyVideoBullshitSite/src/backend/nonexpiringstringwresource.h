#ifndef NONEXPIRINGSTRINGWRESOURCE_H
#define NONEXPIRINGSTRINGWRESOURCE_H

#include <Wt/WServer>
#include <Wt/WResource>
using namespace Wt;

class NonExpiringStringWResource : public WResource
{
public:
    NonExpiringStringWResource(std::string resourceBytes, std::string resourceMimeType, std::string resourceSuggestedFilename, DispositionType dispositionType, WObject* parent = 0);
    void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
    ~NonExpiringStringWResource();
private:
    const std::string dateTimeToRfc1123(const WDateTime &dateTime);
    const std::string m_ResourceBytes;
    const u_int64_t m_ResourceBytesLength;
    const std::string m_ResourceMimeType;
};

#endif // NONEXPIRINGSTRINGWRESOURCE_H
