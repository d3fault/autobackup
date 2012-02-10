#ifndef ADCAPTCHASITE_H
#define ADCAPTCHASITE_H

#include <Wt/WApplication>
#include <Wt/WText>
using namespace Wt;

class AdCaptchaSite : public WApplication
{
public:
    AdCaptchaSite(const WEnvironment &env);
};

#endif // ADCAPTCHASITE_H
