#include "abchome.h"

AbcHome::AbcHome(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WText("Hello Home"));
}

const std::string AbcHome::ReadableText = "Home";
const std::string AbcHome::PreferredInternalPath = "/";

bool AbcHome::isInternalPath(const std::string &internalPath)
{
    //TODOopt: make internal path lowercase before comparing... so the user can type all variations of uppercase and lowercase. ALSO CONSIDER THAT GOOGLE DOESN'T LIKE HAVING MULTIPLE URLS POINTING TO THE SAME CONTENT/PAGE. but fuck google

    if(internalPath == PreferredInternalPath)
        return true;
    if(internalPath == "")
        return true;
    if(internalPath == "/home")
        return true;
    return false;
}
bool AbcHome::requiresLogin()
{
    return false;
}
