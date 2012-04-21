#ifndef ABCHOME_H
#define ABCHOME_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class AbcHome : public WContainerWidget
{
public:
    AbcHome(WContainerWidget *parent = 0);

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
};

#endif // ABCHOME_H
