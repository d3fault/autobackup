#ifndef ABCFIRSTLOGIN_H
#define ABCFIRSTLOGIN_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WPushButton>
using namespace Wt;

class AbcFirstLogin : public WContainerWidget
{
public:
    AbcFirstLogin(WContainerWidget *parent = 0);

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
};

#endif // ABCFIRSTLOGIN_H
