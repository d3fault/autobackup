#ifndef ABCBALANCEREQUESTPAYOUT_H
#define ABCBALANCEREQUESTPAYOUT_H

#include <Wt/WContainerWidget>
#include <Wt/WText>
using namespace Wt;

class AbcBalanceRequestPayout : public WContainerWidget
{
public:
    AbcBalanceRequestPayout(WContainerWidget *parent = 0);

    static const std::string ReadableText;
    static const std::string PreferredInternalPath;

    static bool isInternalPath(const std::string &internalPath);
    static bool requiresLogin();
};

#endif // ABCBALANCEREQUESTPAYOUT_H
