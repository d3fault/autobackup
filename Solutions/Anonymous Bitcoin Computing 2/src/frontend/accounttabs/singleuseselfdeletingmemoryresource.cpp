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
    }
}
void SingleUseSelfDeletingMemoryResource::handleRequest(const Http::Request &request, Http::Response &response)
{
    (void)request;
    //TODOreq: response.setMimeType("plain/text");
    response.out() << *m_MemoryResource; //TODOreq: endl necessary because of http spec? << std::endl;

    //what's the opposite of JIT? well this is it.
    delete m_MemoryResource;
    m_MemoryResource = 0;
}
