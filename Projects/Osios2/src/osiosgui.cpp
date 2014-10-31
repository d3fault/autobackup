#include "osiosgui.h"

#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopWidget>

#include "osiosdhtbootstrapsplashdialog.h"
#include "osiosprofilemanagerdialog.h"
#include "osioscreateprofiledialog.h"
#include "osios.h"
#include "osiosmainwindow.h"
#include "osioscommon.h"
#include "osiossettings.h"

OsiosGui::OsiosGui(QObject *parent)
    : QObject(parent)
    , m_Osios(new Osios(this))
    , m_BootstrapSplashDialog(0)
    , m_MainWindow(0)
{
    connect(m_Osios, SIGNAL(quitRequested()), this, SLOT(quit()));
    m_Osios->initializeAndStart(this);
    //QMetaObject::invokeMethod(m_Osios, "initializeAndStart", Q_ARG(IOsiosDhtBootstrapClient*,this));
}
bool OsiosGui::hasBootstrapSplashScreen()
{
    return true;
}
IOsiosDhtBootstrapSplashScreen *OsiosGui::createAndPresentBootstrapSplashScreen(const QString &lastUsedProfileNameToDisplayInSplash, bool autoLoginLastUsedProfileOnBootstrap)
{
    if(m_BootstrapSplashDialog)
        delete m_BootstrapSplashDialog;
    m_BootstrapSplashDialog = new OsiosDhtBootstrapSplashDialog(lastUsedProfileNameToDisplayInSplash, autoLoginLastUsedProfileOnBootstrap);
    m_BootstrapSplashDialog->setModal(false); //we're on same thread as network, and network needs event loop. Since it's only displayed dialog atm, it doesn't matter...

    connect(m_BootstrapSplashDialog, SIGNAL(finished(int)), m_BootstrapSplashDialog, SLOT(deleteLater())); //pretty sure other listeners of accepted(), such as Osios itself (like seeing whether or not checkbox is [still] checked in splash, but hey i could toggle dat shit with a signal instead to be safer), still have time to access m_BootstrapSplashDialog before it's deleted, so long as those listeners are on the same thread
    connect(m_Osios, SIGNAL(notificationAvailable(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)), m_BootstrapSplashDialog, SLOT(presentNotification(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)));

    m_BootstrapSplashDialog->show();
    return m_BootstrapSplashDialog;
}
bool OsiosGui::hasCreateProfileUi()
{
    return true;
}
bool OsiosGui::showCreateProfileAndReturnWhetherOrNotTheDialogWasAccepted(QString *newlyCreatedProfile)
{
    OsiosCreateProfileDialog createProfileDialog;
    //createProfileDialog.setModal(false); i want to but i don't want profile manager to be usable underneath
    if(createProfileDialog.exec() == QDialog::Accepted)
    {
        *newlyCreatedProfile = createProfileDialog.newProfileName(); //TODOreq: dataDir also. Instead of being passed a QString ptr for my output, I should pass in a Profile ptr
        return true;
    }
    else
    {
        //*newlyCreatedProfile->clear();, better to rely on us returning false :-P
        return false;
    }
}
bool OsiosGui::hasProfileManagerUi()
{
    return true;
}
bool OsiosGui::showProfileManagerAndReturnWhetherItWasAcceptedOrNotWhichTellsUsIfAProfileWasSelectedOrNot(QString *chosenProfileName)
{
    OsiosProfileManagerDialog profileManagerDialog;
    profileManagerDialog.setModal(false);
    if(profileManagerDialog.exec() == QDialog::Accepted) //does this even become non-blocking-ish when setModal(true) occurs? i worry that my dialogs are blocking the event loop... i may be right :-/. whatever i am doing them quick for now lol. TODOreq fix. use heap + signals (just like splash) i guess. HOWEVER it might be fine that we're blocking osios (since he calls us directly), idfk. IT IS NOT OK THAT WE ARE BLOCKING if we want to use a create profile signal to signal profile creation... because then osios can't respond to it until it's done. actually that might be fine, it might still work, but it's still sloppy
    {
        *chosenProfileName = profileManagerDialog.profileNameChosen();
        return true;
    }
    else
    {
        //*stringToSetToChosenProfileName->clear();, better to rely on us returning false :-P
        return false;
    }
}
IOsiosClient *OsiosGui::createMainUi()
{
    if(m_MainWindow)
        delete m_MainWindow;
    m_MainWindow = new OsiosMainWindow(m_Osios);
    return m_MainWindow;
}
void OsiosGui::presentMainUi()
{
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    QByteArray previousResolutionSetting = settings->value("geometry").toByteArray();
    if(previousResolutionSetting.isEmpty())
    {
        QDesktopWidget desktopWidget;
        QSize screenResolution = desktopWidget.availableGeometry().size();
        if(screenResolution.width() >= 1024 && screenResolution.height() >= 768)
        {
            m_MainWindow->resize(1024, 768); //TODOreq: conflicts with QMainWindow::loadState. whatever they last used should be used over this
            m_MainWindow->show();
        }
        else
            m_MainWindow->showMaximized();
    }
    else
    {
        //restoreGeometry() in main window's constructor will use whatever they used last time
        m_MainWindow->show();
    }
}
bool OsiosGui::mainUiHasCopycatAbility()
{
    return true;
}
IOsiosCopycatClient *OsiosGui::mainUiAsCopycatClient()
{
    return m_MainWindow;
}
OsiosGui::~OsiosGui()
{
    if(m_Osios)
    {
        delete m_Osios;
    }
    if(m_MainWindow)
    {
        delete m_MainWindow;
    }
}
void OsiosGui::quit()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
