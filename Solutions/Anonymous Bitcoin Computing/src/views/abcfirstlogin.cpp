#include "abcfirstlogin.h"

AbcFirstLogin::AbcFirstLogin(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WText("This Is Your First Time Logging In. Create Your Bank Account. TODO: Select If You Want It Secure As Fuck Or Not"));
    addWidget(new WPushButton("Create Bank Account"));

    //tbh, even though i've written boatloads of code (which i now need to comment out), i want to test just that my first-login detection is working before i even proceed with connecting that pushbutton to anything. ALWAYS CODE IN BABY STEPS WITH A COMMIT BETWEEN EACH STEP. god wtf are you doing man?
}

const std::string AbcFirstLogin::ReadableText = "First Login";
const std::string AbcFirstLogin::PreferredInternalPath = "/first-login";

bool AbcFirstLogin::isInternalPath(const std::string &internalPath)
{
    if(internalPath == PreferredInternalPath)
        return true;
    if(internalPath == "firstlogin")
        return true;
    return false;
}
bool AbcFirstLogin::requiresLogin()
{
    return true;
}
