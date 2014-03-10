#ifndef HACKYVIDEOBULLSHITSITE_H
#define HACKYVIDEOBULLSHITSITE_H

#include <Wt/WServer>
using namespace Wt;

class BoostThreadInitializationSynchronizationKit;

class HackyVideoBullshitSite
{
public:
    int startHackyVideoBullshitSiteAndWaitForFinished(int argc, char* argv[]);
private:
    void adImageGetAndSubscribeThreadEntryPoint(BoostThreadInitializationSynchronizationKit *threadInitializationSynchronizationKit);
};

#endif // HACKYVIDEOBULLSHITSITE_H
