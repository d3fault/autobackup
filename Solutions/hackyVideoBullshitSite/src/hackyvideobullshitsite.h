#ifndef HACKYVIDEOBULLSHITSITE_H
#define HACKYVIDEOBULLSHITSITE_H

#include <Wt/WServer>
#include <Wt/WApplication>
using namespace Wt;

class HackyVideoBullshitSite
{
public:
    int startHackyVideoBullshitSiteAndWaitForFinished(int argc, char* argv[]);
private:
    static WApplication *hackyVideoBullshitSiteGuiEntryPoint(const WEnvironment &env);
};

#endif // HACKYVIDEOBULLSHITSITE_H
