#include "osiosgui.h"

#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopWidget>

#include "osiosprofilemanagerdialog.h"
#include "osioscreateprofiledialog.h"
#include "osios.h"
#include "osiosmainwindow.h"
#include "osioscommon.h"
#include "osiossettings.h"

OsiosGui::OsiosGui(QObject *parent)
    : QObject(parent)
    , m_Osios(0)
    , m_MainWindow(0)
{
    QStringList argz = QCoreApplication::arguments();
    argz.removeFirst(); //filepath

    bool profileManagerRequested = false;
    int profileManagerArgIndex = argz.indexOf("--profile-manager");
    if(profileManagerArgIndex != -1)
    {
        profileManagerRequested = true;
        argz.removeAt(profileManagerArgIndex);
    }

    QString profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet;
    int profileSpecificationArgIndex = argz.indexOf("--profile");
    if(profileSpecificationArgIndex != -1)
    {
        if((profileSpecificationArgIndex+1) >= argz.size())
        {
            usageAndQuit();
            return;
        }
        profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet = argz.at(profileSpecificationArgIndex+1);
        argz.removeAt(profileSpecificationArgIndex);
        argz.removeAt(profileSpecificationArgIndex);
    }

    quint16 localServerPort_OrZeroToChooseRandomPort = 0;
    int listenPortArgVindex = argz.indexOf("--listen-port");
    if(listenPortArgVindex > -1)
    {
        if((listenPortArgVindex+1) >= argz.size())
        {
            usageAndQuit();
            return;
        }
        bool convertOk = false;
        localServerPort_OrZeroToChooseRandomPort = argz.at(listenPortArgVindex+1).toUShort(&convertOk); //considering making this optional (auto-generated, who gives a fuck. so long as we say what we chose in our status notifications...)
        if(!convertOk)
        {
            usageAndQuit();
            return;
        }
        argz.removeAt(listenPortArgVindex);
        argz.removeAt(listenPortArgVindex);
        if(argz.indexOf("--listen-port") != -1)
        {
            usageAndQuit();
            return;
        }
    }


    qRegisterMetaType<ListOfDhtPeerAddressesAndPorts>("ListOfDhtPeerAddressesAndPorts");
    ListOfDhtPeerAddressesAndPorts bootstrapAddressesAndPorts; //TODOreq: store some (ALL LEARNED?) addresses/ips in settings or something
    while(!argz.isEmpty())
    {
        if(argz.first() != "--add-bootstrap-node")
        {
            usageAndQuit();
            return;
        }
        if(argz.size() < 2)
        {
            usageAndQuit();
            return;
        }
        QUrl hostAndPort = QUrl::fromUserInput(argz.at(1));
        if(!hostAndPort.isValid())
        {
            usageAndQuit();
            return;
        }
        bootstrapAddressesAndPorts.append(qMakePair(QHostAddress(hostAndPort.host()), hostAndPort.port()));
        argz.takeFirst(); //add bootstrap arg
        argz.takeFirst(); //host:port
    }

    QSettings settings;

    if(profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet.isEmpty())
    {
        profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet = settings.value(LAST_USED_PROFILE_SETTINGS_KEY).toString(); //try reading last used profile, but it may also be empty/missing so that would leave the string the same as it was anyways
    }

    if(profileManagerRequested)
    {
        OsiosProfileManagerDialog profileManagerDialog;
        if(profileManagerDialog.exec() == QDialog::Accepted)
        {
            profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet = profileManagerDialog.profileNameChosen();
        }
        else
        {
            //cancel pressed
            quit();
            return;
        }
    }
    else if(profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet.isEmpty())
    {
        OsiosCreateProfileDialog createProfileDialog;
        if(createProfileDialog.exec() != QDialog::Accepted)
        {
            quit(); //TODOoptional: modal dialog infinite loop asking them if they're sure they want to quit or if they want to go to the create profile screen again
            return;
        }
        profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet = createProfileDialog.newProfileName();
    }

    //if we get here, profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet is set and valid
    settings.setValue(LAST_USED_PROFILE_SETTINGS_KEY, profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet);
    m_Osios = new Osios(profileToUse_OrEmptyStringIfNoneDecidedCreatedEtcYet, localServerPort_OrZeroToChooseRandomPort, bootstrapAddressesAndPorts);
    m_MainWindow = new OsiosMainWindow(m_Osios);
    connectBackendToAndFromFrontendSignalsAndSlots();
    showMainWindow();
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
void OsiosGui::usageAndQuit()
{
    //TODOreq: modal dialog? might as well just ask them for the port at that point (but... asking for bootstrap nodes is more work guh)
    QMessageBox::critical(0, tr("Error"), tr("You either didn't pass enough arguments to the app, or you passed invalid arguments.\n\nUsage: Osios localServerPort [--listen-port N --add-bootstrap-node host:port]\n\nIf you don't specify a --listen-port, one is chosen for you at random.\nYou can supply --add-bootstrap-node multiple times"));
    quit();
}
void OsiosGui::quit()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void OsiosGui::connectBackendToAndFromFrontendSignalsAndSlots()
{
    //TODOreq: every timeline node action that we want to serialize needs a connection. change tab, key press, etc
    connect(m_MainWindow, SIGNAL(actionOccurred(TimelineNode)), m_Osios, SLOT(recordMyAction(TimelineNode))); //old: called tab in frontend and activity on backend because there might be a cli version someday

    connect(m_Osios, SIGNAL(connectionColorChanged(int)), m_MainWindow, SLOT(changeConnectionColor(int)));
    connect(m_Osios, SIGNAL(notificationAvailable(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)), m_MainWindow, SIGNAL(presentNotificationRequested(QString,OsiosNotificationLevels::OsiosNotificationLevelsEnum)));
}
void OsiosGui::showMainWindow()
{
    QSettings settings;
    QByteArray previousResolutionSetting = settings.value("geometry").toByteArray();
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
