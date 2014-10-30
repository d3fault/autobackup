#ifndef OSIOSGUI_H
#define OSIOSGUI_H

#include <QObject>
#include "iosiosdhtbootstrapclient.h"

class Osios;
class OsiosDhtBootstrapSplashDialog;
class OsiosMainWindow;

class OsiosGui : public QObject, public IOsiosDhtBootstrapClient
{
    Q_OBJECT
public:
    explicit OsiosGui(QObject *parent = 0);

    bool hasBootstrapSplashScreen();
    IOsiosDhtBootstrapSplashScreen *createAndPresentBootstrapSplashScreen(const QString &lastUsedProfileNameToDisplayInSplash, bool autoLoginLastUsedProfileOnBootstrap);

    bool hasCreateProfileUi();
    bool showCreateProfileAndReturnWhetherOrNotTheDialogWasAccepted(QString *newlyCreatedProfile);

    bool hasProfileManagerUi();
    bool showProfileManagerAndReturnWhetherItWasAcceptedOrNotWhichTellsUsIfAProfileWasSelectedOrNot(QString *chosenProfileName);

    IOsiosClient *createMainUi();
    void presentMainUi();

    bool mainUiHasCopycatAbility();
    IOsiosCopycatClient *mainUiAsCopycatClient();

    ~OsiosGui();
private:
    Osios *m_Osios;
    OsiosDhtBootstrapSplashDialog *m_BootstrapSplashDialog;
    OsiosMainWindow *m_MainWindow;
private slots:
    void quit();
};

#endif // OSIOSGUI_H
