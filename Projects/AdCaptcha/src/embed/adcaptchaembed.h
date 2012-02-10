#ifndef ADCAPTCHAEMBED_H
#define ADCAPTCHAEMBED_H

//TODO: maybe this should be WidgetSet?? idfk... there's like no documentation on it. WidgetSet isn't here... but Widget is... idk.
#include <Wt/WApplication>
using namespace Wt;

class AdCaptchaEmbed : public WApplication
{
public:
    AdCaptchaEmbed(const WEnvironment &env);
};

#endif // ADCAPTCHAEMBED_H
