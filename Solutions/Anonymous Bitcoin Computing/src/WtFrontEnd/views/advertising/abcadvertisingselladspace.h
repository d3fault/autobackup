#ifndef ABCADVERTISINGSELLADSPACE_H
#define ABCADVERTISINGSELLADSPACE_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class AbcAdvertisingSellAdSpace : public WContainerWidget
{
public:
    AbcAdvertisingSellAdSpace(WContainerWidget *parent = 0);

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
};

#endif // ABCADVERTISINGSELLADSPACE_H
