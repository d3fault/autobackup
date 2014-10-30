#ifndef IOSIOSDHTBOOTSTRAPCLIENT_H
#define IOSIOSDHTBOOTSTRAPCLIENT_H

#include <QString>

class IOsiosDhtBootstrapSplashScreen;
class IOsiosClient;
class IOsiosCopycatClient;

class IOsiosDhtBootstrapClient
{
public:
    virtual bool hasBootstrapSplashScreen()=0;
    virtual IOsiosDhtBootstrapSplashScreen *createAndPresentBootstrapSplashScreen(const QString &lastUsedProfileNameToDisplayInSplash, bool autoLoginLastUsedProfileOnBootstrap)=0;

    virtual bool hasCreateProfileUi()=0;
    virtual bool showCreateProfileAndReturnWhetherOrNotTheDialogWasAccepted(QString *newlyCreatedProfile)=0;

    virtual bool hasProfileManagerUi()=0;
    virtual bool showProfileManagerAndReturnWhetherItWasAcceptedOrNotWhichTellsUsIfAProfileWasSelectedOrNot(QString *chosenProfileName)=0;

    virtual IOsiosClient *createMainUi()=0;
    virtual void presentMainUi()=0;

    virtual bool mainUiHasCopycatAbility()=0;
    virtual IOsiosCopycatClient *mainUiAsCopycatClient()=0;
};

#endif // IOSIOSDHTBOOTSTRAPCLIENT_H
