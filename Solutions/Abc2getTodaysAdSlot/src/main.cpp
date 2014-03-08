#include <Wt/WServer>

#include <iostream>

#include "gettodaysadslotwresource.h"

using namespace Wt;
using namespace std;

int main(int argc, char* argv[])
{
    WServer serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi(argv[0]);
    serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    GetTodaysAdSlotWResource getTodaysAdSlotResource;
    serverWithOnlyGlobalResourcesOneForEachAdSlotOrSomethingAlongThoseLinesHi.addResource(&getTodaysAdSlotResource, "/getTodaysAdSlot"); //proper? does server take ownership? if not, scope will take care of it :)

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
