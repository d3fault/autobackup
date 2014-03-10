#include "adimagewresource.h"

#include <Wt/Http/Response>
#include <Wt/WDateTime>

//doesn't make sense for this to be "backend", but also doesn't NEED to be front end (abstract WResource pointer is "enough"). need to refactor my backend to not depend on anything Wt related, but don't give nearly enough of a fuck to do that now...
AdImageWResource::AdImageWResource(const std::string &adImageBytes, const std::string &adImageMimeType, const std::string &adImageSuggestedFilename, DispositionType dispositionType, WObject *parent)
    : WResource(parent), m_AdImageBytes(adImageBytes), m_AdImageMimeType(adImageMimeType), m_AdImageBytesLength(adImageBytes.length())
{
    suggestFileName(adImageSuggestedFilename, dispositionType);
}
//TODOoptimization: maybe make this a global public resource, so that the expire date actually does something. if the user closes browser and re-opens it, they'll have a new wt session and i think that means they'd re-request the resource, but am unsure (pretty sure though)...
void AdImageWResource::handleRequest(const Http::Request &request, Http::Response &response)
{
    (void)request;
    WDateTime now = WDateTime::currentDateTime();
    response.addHeader("Date", dateTimeToRfc1123(now));
    WDateTime oneYearFromNow = now.addYears(1); //TODOoptional: use some value based on slot length hours (maybe twice as long as a slot length). setting to a year for now won't hurt. and we'll have long since stopped referring to the image by then anyways (but that's not true for all possible values of slot length hours)...
    response.addHeader("Expires", dateTimeToRfc1123(oneYearFromNow));
    response.setContentLength(m_AdImageBytesLength);
    response.setMimeType(m_AdImageMimeType);
    response.out() << m_AdImageBytes; //TODOreqoptimization: maybe response continuations with an optimial(?) chunk size? i really don't know what that optimal size is, but i do know that how i have it coded right now makes a full copy of the image the moment they request it (FOR EACH REQUEST BTW, ~10k*164kb = 1.64gb... no... that can't be right... wtf...), so it might be just a great memory usage optimization to use response continuations (too small and it's a performance hit [maybe (likely)]). My 'span out WServer::posts over 5 mins' is mainly to optimize bandwidth, and it might solve this problem too (but really a response continuation isn't THAT hard)
}
AdImageWResource::~AdImageWResource()
{
    beingDeleted();
}
//copy/paste job from Abc2's SingleUseSelfDeletingMemoryResource, merge into some shared file if you're bored...
//expects/demands UTC (WDateTime::currentDateTime() complies)
const std::string AdImageWResource::dateTimeToRfc1123(const WDateTime &dateTime)
{
    return dateTime.toString("ddd, dd MMM yyyy hh:mm:ss GMT").toUTF8();
}
