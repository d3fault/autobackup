#include "osiosgui.h"

#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>
#include <QUrl>

#include "osioscreateprofiledialog.h"
#include "osios.h"
#include "osiosmainwindow.h"
#include "osioscommon.h"

#define LAST_USED_PROFILE_SETTINGS_KEY "lastUsedProfile"
#define NUM_PROFILES_SETTINGS_KEY "numProfiles"

OsiosGui::OsiosGui(QObject *parent)
    : QObject(parent)
    , m_Osios(0)
    , m_MainWindow(0)
{
    QStringList argz = QCoreApplication::arguments();
    if(argz.size() < 2)
    {
        usageAndQuit();
        return;
    }
    bool convertOk = false;
    quint16 localServerPort = argz.at(1).toUShort(&convertOk); //considering making this optional (auto-generated, who gives a fuck. so long as we say what we chose in our status notifications...)
    if(!convertOk)
    {
        usageAndQuit();
        return;
    }

    argz.removeFirst(); //filepath
    argz.removeFirst(); //port


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
    }

    QSettings settings;
    QString lastUsedProfile_OrEmptyStringIfNone = settings.value(LAST_USED_PROFILE_SETTINGS_KEY).toString();

    //there still might be profiles, so we would offer for them to select existing ideally (with option to create still ofc)
    int numProfiles = settings.value(NUM_PROFILES_SETTINGS_KEY).toInt();
    if(numProfiles > 0)
    {
        settings.beginGroup(PROFILES_GOUP_SETTINGS_KEY);
        //add to some list for combo box
        settings.endGroup();
    }
    //TODOreq: present "choose existing profile" dialog, including setting lastUsedProfile_OrEmptyStringIfNone to the chosen profile before proceeding forward


    if(lastUsedProfile_OrEmptyStringIfNone.isEmpty())
    {
        OsiosCreateProfileDialog createProfileDialog;
        while(createProfileDialog.newProfileName().trimmed().isEmpty())
        {
            if(createProfileDialog.exec() != QDialog::Accepted)
            {
                QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection); //TODOoptional: modal dialog infinite loop asking them if they're sure they want to quit or if they want to go to the create profile screen again
                return;
            }
            if(createProfileDialog.newProfileName().trimmed().isEmpty())
            {
                QMessageBox::critical(0, tr("Error"), tr("Error: Profile Name Cannot Be Empty"));
            }
        }
        lastUsedProfile_OrEmptyStringIfNone = createProfileDialog.newProfileName();

        settings.beginGroup(PROFILES_GOUP_SETTINGS_KEY);
        //TODOreq: make sure the profile name doesn't already exist in the settings
        settings.beginGroup(lastUsedProfile_OrEmptyStringIfNone);
        settings.setValue(OSIOS_DATA_DIR_SETTINGS_KEY, createProfileDialog.chosenDataDir());
        settings.endGroup();
        settings.endGroup();
        settings.setValue(LAST_USED_PROFILE_SETTINGS_KEY, lastUsedProfile_OrEmptyStringIfNone);
    }

    m_Osios = new Osios(lastUsedProfile_OrEmptyStringIfNone, localServerPort, bootstrapAddressesAndPorts);
    m_MainWindow = new OsiosMainWindow(m_Osios);
    connectBackendToAndFromFrontendSignalsAndSlots();
    m_MainWindow->show();
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
    QMessageBox::critical(0, tr("Error"), tr("You either didn't pass enough arguments to the app, or you passed invalid arguments.\n\nUsage: Osios localServerPort [--add-bootstrap-node host:port]\n\nYou can supply --add-bootstrap-node multiple times"));
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void OsiosGui::connectBackendToAndFromFrontendSignalsAndSlots()
{
    //TODOreq: every timeline node action that we want to serialize needs a connection. change tab, key press, etc
    connect(m_MainWindow, SIGNAL(actionOccurred(TimelineNode)), m_Osios, SLOT(recordMyAction(TimelineNode))); //old: called tab in frontend and activity on backend because there might be a cli version someday

    void connectionColorChanged(int color);
    void notificationAvailable(QString notificationMessage, OsiosNotificationLevels::OsiosNotificationLevelsEnum notificationLevel = OsiosNotificationLevels::StandardNotificationLevel);
}
