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
