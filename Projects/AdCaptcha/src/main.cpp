#define WT_AC_ENTRY_POINT_SITE "/home"
//also define about, register, login, etc etc

#define WT_AC_ENTRY_POINT_EMBED "/adcaptcha"

#include <Wt/WServer>
using namespace Wt;

#include "site/adcaptchasite.h"
#include "embed/adcaptchaembed.h"

WApplication *createSiteApplication(const WEnvironment &env)
{
    return new AdCaptchaSite(env);
}
WApplication *createEmbedApplication(const WEnvironment &env)
{
    return new AdCaptchaEmbed(env);
}
int main(int argc, char **argv)
{
    WServer server(argv[0]);

    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    server.addEntryPoint(Application, &createSiteApplication, WT_AC_ENTRY_POINT_SITE);
    server.addEntryPoint(WidgetSet, &createEmbedApplication, WT_AC_ENTRY_POINT_EMBED);

    if(server.start())
    {
        int ret = server.waitForShutdown();
        server.stop();
        return ret;
    }
    else
        return 1;
}
