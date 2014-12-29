#include <Wt/WServer>

#include <iostream>

#include "gettodaysadslotwresource.h"

using namespace Wt;
using namespace std;

int main(int argc, char* argv[])
{
    WServer serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi(argv[0]); //I don't think two WServers can be active at once (even if different processes) when they are both binding to the same port! I need to merge this synchronous pos into Abc2 itself :( -- either that or I use a different port, which isn't such a bad option (I think my ssl cert works on other ports? idfk)
    serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    GetTodaysAdSlotWResource getTodaysAdSlotResource;
    if(!getTodaysAdSlotResource.initialize())
    {
        cerr << "GetTodaysAdSlotWResource failed to initialize" << endl;
        return 1;
    }
    serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.addResource(&getTodaysAdSlotResource, "/todays-ad.json"); //proper? does server take ownership? if not, scope will take care of it :)

    int ret = 0;
    if(serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.start())
    {
        ret = serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.waitForShutdown();
        if(ret == 2 || ret == 3 || ret == 15) //SIGINT, SIGQUIT, SIGTERM, respectively (TODOportability: are these the same on windows?). Any other signal we return verbatim
        {
            ret = 0;
        }
        serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.stop();
    }
    else
    {
        ret = 1;
    }
    if(ret != 0)
    {
        cerr << argv[0] << " not exitting with return code zero: " << ret << endl;
    }
    else
    {
        cout << argv[0] << " exitting cleanly" << endl;
    }
    return ret;
}
