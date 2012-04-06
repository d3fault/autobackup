#include <Wt/WApplication>
using namespace Wt;

#include "database.h"
#include "anonymousbitcoincomputing.h"

WApplication *serverEntryPoint(const WEnvironment &env)
{
    return new AnonymousBitcoinComputing(env);
}
int main(int argc, char **argv)
{
    Database::GlobalConfigure();

    //Database processWideDb; //so sqlite doesn't close the file while there are no users or something? the dbo tutorial says to do this. i was originally planning to give this to Qt to use... but not anymore now that the AppDb is an SSL session away~

    //the Qt "AppDbHelper" has an in-memory cache, and a qt sqlite cache (because using a bunch of memory will limit our connections amount!). the actual AppDb pushes all updates to us... so we only need to get the 'latest and greates' (this could be really inefficient later on) right at startup.......... OR we can just pull if it doesn't exist in our own cache... but if it is in our cache, we know it's up-to-date because we always get pushed updates (this is much more efficient)
    //i want to throw up at how complicated this is getting

    return WRun(argc, argv, &serverEntryPoint);

    /*WServer server(argv[0]);

    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    server.addEntryPoint(Application, &serverEntryPoint);

    Database::GlobalConfigure();

    if(server.start())
    {
        int ret = server.waitForShutdown();
        server.stop();
        return ret;
    }
    else
        return 1;
        */
}
