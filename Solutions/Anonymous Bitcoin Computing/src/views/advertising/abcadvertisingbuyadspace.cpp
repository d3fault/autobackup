#include "abcadvertisingbuyadspace.h"

AbcAdvertisingBuyAdSpace::AbcAdvertisingBuyAdSpace(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WText("Hello Buy Ad Space"));
}
const std::string AbcAdvertisingBuyAdSpace::ReadableText = "Buy Ad Space";
const std::string AbcAdvertisingBuyAdSpace::PreferredInternalPath = "/advertising/buy";

bool AbcAdvertisingBuyAdSpace::isInternalPath(const std::string &internalPath)
{
    if(internalPath == PreferredInternalPath)
    {
        return true;
    }
    if(internalPath == "/ads/buy")
        return true;
    if(internalPath == "/ad/buy")
        return true;
    //etc. infinite aliasing
    return false;
}
bool AbcAdvertisingBuyAdSpace::requiresLogin()
{
    return false; //maybe should be true... or/also maybe should i show the page and just make certain widgets readOnly? and of course whatever slot they're (they'd be) connected to checks again for logged in...

    //a page like "balance" and "add funds" etc DEFINITELY requiresLogin... but this page could be partial requires login (aka false right here)
}
