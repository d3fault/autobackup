//#define WT_AC_ENTRY_POINT_SITE "/home"
//also define about, register, login, etc etc

#define WT_AC_ENTRY_POINT_EMBED "/adcaptcha"
//TODO:^ move to AdCaptchaEmbed as static member, because we need it from within there to handle internal path changed

#include <Wt/WServer>
using namespace Wt;

#include "database.h"
#include "site/adcaptchasite.h"
#include "embed/adcaptchaembed.h"

WApplication *createSiteApplication(const WEnvironment &env)
{
    WApplication *userSiteInstance = new WApplication(env);
    userSiteInstance->setTitle("AdCaptcha");

    new AdCaptchaSite(userSiteInstance->root()); //anon, root() takes ownership and handles deletion. we dgaf

    return userSiteInstance;
}
WApplication *createEmbedApplication(const WEnvironment &env)
{
    //todo: figure out WidgetSets and also change AdCaptchaEmbed to be a ContainerWidget just like createSiteApplication above...
    return new AdCaptchaEmbed(env);
}
int main(int argc, char **argv)
{
    WServer server(argv[0]);

    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    server.addEntryPoint(Application, &createSiteApplication);
    server.addEntryPoint(WidgetSet, &createEmbedApplication, WT_AC_ENTRY_POINT_EMBED);

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
