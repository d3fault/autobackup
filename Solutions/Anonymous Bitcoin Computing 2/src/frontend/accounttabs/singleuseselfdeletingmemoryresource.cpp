#include "singleuseselfdeletingmemoryresource.h"

SingleUseSelfDeletingMemoryResource::SingleUseSelfDeletingMemoryResource(const string &data, WObject *parent)
    : WResource(parent), m_MemoryResource(new std::string())
{
    //TODOreq: suggest file name and mime type and all that jazz just like FileDeleting[...]
    *m_MemoryResource = data; //TODOreq: does deep copy (even though less efficient, i want it to)
}
SingleUseSelfDeletingMemoryResource::~SingleUseSelfDeletingMemoryResource()
{
    beingDeleted(); //TODOreq: not exactly sure what this implies and/or if it's safe for me to delete my resource in a few lines (probably IS NOT), so to be on the safe side: a SingleUseSelfDeletingMemoryResource can only be used ONCE (which should seem obvious by the class name anyways)

    //it may have never been used once, so the resource might still be alive and we should delete it here/now
    if(m_MemoryResource)
    {
        delete m_MemoryResource;
        //TODOreq: what if the WApplication (of which 'this' is a child) gets deleted while the [concurrent] resource request is in progress? I'd imagine beingDeleted() has something to do with that, but the documentation doesn't really say HOW it protects against it. I'm worried about threading issues: request on one thread tries to read it and wapplication gets deleted for whatever reason and bam segfault. simply setting it to zero after deleting it here and then checking it isn't zero before streaming it in handleRequest won't do it, because it could be deleted while DOING the streaming out itself (after the check), due to the nature of multi-threading xD.
        //^For now I'm just going to say fuck it, but as a hacky last resort I could use a mutex to protect access to m_MemoryResource here and in handleRequest....

        m_MemoryResource = 0; //doesn't solve above problem
    }
}
void SingleUseSelfDeletingMemoryResource::handleRequest(const Http::Request &request, Http::Response &response)
{
    (void)request;
    if(m_MemoryResource) //making top most in stacked widget for some reason re-requests this. hopefully the caching below will help, but we still should do the check in case the browser doesn't comply (never let your app segfault when a browser doesn't feel like complying). initial testing seems to indicate that the cache headers solved it :)
    {
        //TODOreq (proper): response.setMimeType("plain/text");
        WDateTime now = WDateTime::currentDateTime();
        response.addHeader("Date", dateTimeToRfc1123(now)); //does wt handle the colon?
        WDateTime oneYearFromNow = now.addYears(1);
        response.addHeader("Expires", dateTimeToRfc1123(oneYearFromNow));
        response.setContentLength(m_MemoryResource->length());
        response.setMimeType("image/png");
        response.out() << *m_MemoryResource;

        //what's the opposite of JIT? well this is it.
        delete m_MemoryResource;
        m_MemoryResource = 0;
    }
}
//expects/demands UTC (WDateTime::currentDateTime() complies), there's no 'letters' timezone format thingo, only numbers one :(
const string &SingleUseSelfDeletingMemoryResource::dateTimeToRfc1123(const WDateTime &dateTime)
{
    return dateTime.toString("ddd, dd MMM yyyy hh:mm:ss GMT").toUTF8(); //hacked together RFC 1123 (that wasn't so bad)
}
