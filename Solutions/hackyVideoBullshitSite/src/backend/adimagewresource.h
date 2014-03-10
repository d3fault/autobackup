#ifndef ADIMAGEWRESOURCE_H
#define ADIMAGEWRESOURCE_H

#include <Wt/WResource>
using namespace Wt;

class AdImageWResource : public WResource
{
public:
    AdImageWResource(const std::string &adImageBytes, const std::string &adImageMimeType, const std::string &adImageSuggestedFilename, DispositionType dispositionType, WObject* parent = 0);
    void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
    ~AdImageWResource();
private:
    const std::string dateTimeToRfc1123(const WDateTime &dateTime);
    const std::string m_AdImageBytes;
    const u_int64_t m_AdImageBytesLength;
    const std::string m_AdImageMimeType;
};

#endif // ADIMAGEWRESOURCE_H
