#include "adcaptchasite.h"

AdCaptchaSite::AdCaptchaSite(const WEnvironment &env)
    : WApplication(env)
{
    setTitle("Frosted Butts");
    new WText("Hello Wt", root());
}
