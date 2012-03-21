#include <Wt/WServer>
using namespace Wt;

#include "database.h"

#include "anonymousbitcoincomputing.h"


WApplication *serverEntryPoint(const WEnvironment &env)
{
    WApplication *userSiteInstance = new WApplication(env);
    userSiteInstance->setTitle("Anonymous Bitcoin Computing");

    new AnonymousBitcoinComputing(userSiteInstance->root()); //anon, root() takes ownership and handles deletion. we dgaf

    return userSiteInstance;
}

int main(int argc, char **argv)
{
    WServer server(argv[0]);

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
}
