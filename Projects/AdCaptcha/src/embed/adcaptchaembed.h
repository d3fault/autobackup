#ifndef ADCAPTCHAEMBED_H
#define ADCAPTCHAEMBED_H

//TODO: maybe this should be WidgetSet?? idfk... there's like no documentation on it. WidgetSet isn't here... but Widget is... idk.
//note: while we'll store a list of all the current captchas that are sent to the users (for verification of their guess), we have no need to store them in a db. we have no need to permanently store the results (but could if we wanted to). we don't need to store the image either (although the AD half of it should of course be stored/cached)
#include <Wt/WApplication>
using namespace Wt;

class AdCaptchaEmbed : public WApplication
{
public:
    AdCaptchaEmbed(const WEnvironment &env);
};

#endif // ADCAPTCHAEMBED_H
