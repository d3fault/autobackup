#include "abcadvertisingselladspace.h"

AbcAdvertisingSellAdSpace::AbcAdvertisingSellAdSpace(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WText("Hello Sell Ad Space"));
}

const std::string AbcAdvertisingSellAdSpace::ReadableText = "Sell Ad Space";
const std::string AbcAdvertisingSellAdSpace::PreferredInternalPath = "/advertising/sell";

bool AbcAdvertisingSellAdSpace::isInternalPath(const std::string &internalPath)
{
    if(internalPath == PreferredInternalPath)
        return true;
    if(internalPath == "/ads/sell")
        return true;
    if(internalPath == "/ad/sell")
        return true;
    return false;
}
bool AbcAdvertisingSellAdSpace::requiresLogin()
{
    return false;
}
