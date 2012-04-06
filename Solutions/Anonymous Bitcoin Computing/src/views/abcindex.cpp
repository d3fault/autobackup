#include "abcindex.h"

AbcIndex::AbcIndex(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WText("Hello Index"));
}

const std::string AbcIndex::ReadableText = "Index";
const std::string AbcIndex::PreferredInternalPath = "/index";

bool AbcIndex::isInternalPath(const std::string &internalPath)
{
    if(internalPath == PreferredInternalPath)
        return true;
    if(internalPath == "/list")
        return true;
    if(internalPath == "/listings")
        return true;
    return false;
}
bool AbcIndex::requiresLogin()
{
    return false;
}
