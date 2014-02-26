#ifndef SINGLEUSESELFDELETINGMEMORYRESOURCE_H
#define SINGLEUSESELFDELETINGMEMORYRESOURCE_H

#include <Wt/WResource>
#include <Wt/Http/Response>
#include <Wt/WDateTime>

using namespace Wt;
using namespace std;

class SingleUseSelfDeletingMemoryResource : public WResource
{
public:
    SingleUseSelfDeletingMemoryResource(const std::string &data, const string &suggestedFilename = "data.txt", const string &mimeType = "plain/text", DispositionType dispositionType = Attachment, WObject* parent = 0);
    ~SingleUseSelfDeletingMemoryResource();
    virtual void handleRequest(const Http::Request &request, Http::Response &response);
private:
    std::string *m_MemoryResource;
    std::string m_MimeType;
    const std::string dateTimeToRfc1123(const WDateTime &dateTime);
};

#endif // SINGLEUSESELFDELETINGMEMORYRESOURCE_H
