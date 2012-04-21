#ifndef ABCINDEX_H
#define ABCINDEX_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class AbcIndex : public WContainerWidget
{
public:
    AbcIndex(WContainerWidget *parent = 0);

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
};

#endif // ABCINDEX_H
