#include "osiosprofilemanagerdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSettings>
#include <QHBoxLayout>
#include <QPushButton>

#include "osioscreateprofiledialog.h"
#include "osioscommon.h"
#include "osiossettings.h"

const int OsiosProfileManagerDialog::ComboBoxIndexThatIndicatesUnchosen = 0;

//TODOreq: abstract profile managed application. It simply guarantees the application a valid string to indicate a profile (perhaps a pointer to the profile itself, perhaps not). TOo many times (I fixed the typo then put it back) have I wanted to use a create-profile/select-profile/manager-profile system in my app and had been too lazy to fucking do it. Now that I'm implementing this one it's not so bad but I should still abstract it for future apps. It could become a breeze to use profiles in your app
OsiosProfileManagerDialog::OsiosProfileManagerDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(OSIOS_HUMAN_READABLE_TITLE + tr(" - Profile Manager"));

    QHBoxLayout *myLayout = new QHBoxLayout();

    QLabel *existingProfilesLabel = new QLabel(tr("Existing Profiles:"));
    m_ExistingProfilesComboBox = new QComboBox(); //TODOoptional: label to the left saying "existing profiles"
    m_ExistingProfilesComboBox->addItem(tr("Select Profile")); //correlates with ComboBoxIndexThatIndicatesUnchosen. TODOoptional: when there was a last used profile in settings, default to it in the combo box

    //show existing profiles for them to select
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    QStringList allProfiles = settings->value(ALL_PROFILE_NAMES_LIST_SETTINGS_KEY).toStringList();
    m_ExistingProfilesComboBox->addItems(allProfiles);

    QString lastUsedProfile = settings->value(LAST_USED_PROFILE_SETTINGS_KEY).toString();
    if(!lastUsedProfile.isEmpty())
    {
        m_ExistingProfilesComboBox->setCurrentText(lastUsedProfile);
    }

    QVBoxLayout *buttonsColumn = new QVBoxLayout();

    m_OkButton = new QPushButton(tr("Ok"));
    QPushButton *createProfileButton = new QPushButton(tr("Create Profile"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    buttonsColumn->addWidget(m_OkButton);
    buttonsColumn->addWidget(createProfileButton);
    buttonsColumn->addWidget(cancelButton);

    myLayout->addWidget(existingProfilesLabel, 0, Qt::AlignTop);
    myLayout->addWidget(m_ExistingProfilesComboBox, 0, Qt::AlignTop);
    myLayout->addLayout(buttonsColumn);

    setLayout(myLayout);

    connect(createProfileButton, SIGNAL(clicked()), this, SLOT(showCreateProfileDialog()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_OkButton, SIGNAL(clicked()), this, SLOT(accept()));


    m_OkButton->setDefault(true);
    if(m_ExistingProfilesComboBox->count() == 1)
    {
        //no existing profiles were added to the combo box, so disable the ok button
        m_OkButton->setDisabled(true);
        m_OkButton->setToolTip(tr("You need to create a profile first"));
    }
    connect(m_ExistingProfilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableOrDisableOkButtonBasedOnIndexValidity(int)));
}
QString OsiosProfileManagerDialog::profileNameChosen() const //only valid after exec() returns Accepted
{
    return m_ExistingProfilesComboBox->currentText();
}
void OsiosProfileManagerDialog::enableOrDisableOkButtonBasedOnIndexValidity(int newExistingUsersComboBoxIndex)
{
    if(newExistingUsersComboBoxIndex == ComboBoxIndexThatIndicatesUnchosen)
    {
        m_OkButton->setDisabled(true);
    }
    else
    {
        m_OkButton->setEnabled(true);
    }
}
void OsiosProfileManagerDialog::showCreateProfileDialog()
{
    OsiosCreateProfileDialog createProfileDialog;
    if(createProfileDialog.exec() == QDialog::Accepted)
    {
        //add new user to our combo box and select it
        m_ExistingProfilesComboBox->addItem(createProfileDialog.newProfileName());
        m_ExistingProfilesComboBox->setCurrentText(createProfileDialog.newProfileName());
    }
}
