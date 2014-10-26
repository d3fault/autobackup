#include "osiosgui.h"

#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>

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

    m_Osios = new Osios(lastUsedProfile_OrEmptyStringIfNone);
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
void OsiosGui::connectBackendToAndFromFrontendSignalsAndSlots()
{
    //TODOreq: every timeline node action that we want to serialize needs a connection. change tab, key press, etc
    connect(m_MainWindow, SIGNAL(actionOccurred(ITimelineNode)), m_Osios, SLOT(recordAction(ITimelineNode))); //old: called tab in frontend and activity on backend because there might be a cli version someday
}
