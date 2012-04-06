#ifndef ABCADVERTISINGBUYADSPACE_H
#define ABCADVERTISINGBUYADSPACE_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class AbcAdvertisingBuyAdSpace : public WContainerWidget
{
public:
    AbcAdvertisingBuyAdSpace(WContainerWidget *parent = 0);

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static const std::string &getPreferredInternalPath();
    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
};

#endif // ABCADVERTISINGBUYADSPACE_H
