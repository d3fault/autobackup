#ifndef ADCAPTCHASITEHOME_H
#define ADCAPTCHASITEHOME_H

#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WVBoxLayout>
using namespace Wt;

#include "adcaptchasite.h"

class AdCaptchaSiteHome : public WContainerWidget
{
public:
    AdCaptchaSiteHome(WContainerWidget *parent = 0);
};

#endif // ADCAPTCHASITEHOME_H
