#include "osioscreateprofiledialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

#include "labellineeditbrowsebutton.h"
#include "osiossettings.h"
#include "osioscommon.h"

#define  PROFILE_NAME_PERCENT_FILTER "%ProfileName%"

OsiosCreateProfileDialog::OsiosCreateProfileDialog(QWidget *parent)
    : QDialog(parent)
    , m_ProfileNameLineEdit(new QLineEdit())
    , m_DataDirRow(new LabelLineEditBrowseButton(tr("Data Dir:"), LabelLineEditBrowseButton::OpenDirectoryMode))
{
    setWindowTitle(OSIOS_HUMAN_READABLE_TITLE + tr(" - Create Profile"));

    QVBoxLayout *myLayout = new QVBoxLayout();
    myLayout->setContentsMargins(OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS, OSIOS_GUI_LAYOUT_CONTENT_MARGINS);

    QHBoxLayout *profileNameRow = new QHBoxLayout();
    profileNameRow->addWidget(new QLabel(tr("Profile Name:")));
    profileNameRow->addWidget(m_ProfileNameLineEdit);

    QHBoxLayout *cancelOkRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    m_OkButton = new QPushButton(tr("Create Profile"));
    cancelOkRow->addWidget(cancelButton);
    cancelOkRow->addWidget(m_OkButton);

    myLayout->addLayout(profileNameRow);
    myLayout->addLayout(m_DataDirRow);
    myLayout->addLayout(cancelOkRow);
    setLayout(myLayout);

    m_OkButton->setDefault(true);

    connect(m_ProfileNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableOrDisableOkButtonDependingOnContentsValidate()));
    connect(m_DataDirRow->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(enableOrDisableOkButtonDependingOnContentsValidate()));
    //all properties changes should be connected to enableOrDisableOkButtonDependingOnContentsValidate
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_OkButton, SIGNAL(clicked()), this, SLOT(createProfileAndAcceptDialog()));


    m_ProfileNameLineEdit->setPlaceholderText(tr("Enter a Profile Name"));
    m_ProfileNameLineEdit->setFocus();
    m_DataDirRow->lineEdit()->setText(defaultDataDirectory()); //TODOreq: if this is still the contents after the dialog is accepted (or i guess when we're seeing if it will be accepted), we create the directory recursively. No other directories do we create (that's a job for the OS file dialog)
    m_OkButton->setDisabled(true);
}
QString OsiosCreateProfileDialog::newProfileName() const
{
    return m_ProfileNameLineEdit->text();
}
#if 0 //I grab it out of QSettings instead
QString OsiosCreateProfileDialog::chosenDataDir() const
{
    return m_DataDirRow->lineEdit()->text();
}
#endif
const QString &OsiosCreateProfileDialog::defaultDataDirectory() const
{
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    QFileInfo settingsFileInfo(settings->fileName());
    static const QString defaultDataDirectoryThe = settingsFileInfo.absolutePath() + QDir::separator() + PROFILES_GOUP_SETTINGS_KEY + QDir::separator() + PROFILE_NAME_PERCENT_FILTER;
    return defaultDataDirectoryThe;
}
QString OsiosCreateProfileDialog::dataDirectoryResolved()
{
    //return defaultDataDirectory().replace(PROFILE_NAME_PERCENT_FILTER, m_ProfileNameLineEdit->text());
    QString ret = m_DataDirRow->lineEdit()->text();
    ret = ret.replace(PROFILE_NAME_PERCENT_FILTER, m_ProfileNameLineEdit->text());
    return ret;
}
bool OsiosCreateProfileDialog::profileContentsValid()
{
    if(m_ProfileNameLineEdit->text().trimmed().isEmpty())
        return false;
    if(m_DataDirRow->lineEdit()->text().trimmed().isEmpty())
        return false;

    if((m_DataDirRow->lineEdit()->text() == defaultDataDirectory()) || QFileInfo(dataDirectoryResolved()).isDir())
    {
        return true;
    }
    return false;
}
void OsiosCreateProfileDialog::enableOrDisableOkButtonDependingOnContentsValidate()
{
    //if profile valid, ok button enables. if profile invalid, ok button disables

    if(profileContentsValid())
        m_OkButton->setEnabled(true);
    else
        m_OkButton->setDisabled(true);
}
void OsiosCreateProfileDialog::createProfileAndAcceptDialog()
{
    //profile already known to be valid
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    QStringList existingProfileNames =  settings->value(ALL_PROFILE_NAMES_LIST_SETTINGS_KEY).toStringList();
    existingProfileNames.append(m_ProfileNameLineEdit->text());
    settings->setValue(ALL_PROFILE_NAMES_LIST_SETTINGS_KEY, existingProfileNames);

    settings->beginGroup(PROFILES_GOUP_SETTINGS_KEY);
    //TODOreq: make sure the profile name doesn't already exist in the settings
    settings->beginGroup(m_ProfileNameLineEdit->text());
    settings->setValue(OSIOS_DATA_DIR_SETTINGS_KEY, dataDirectoryResolved());
    if(m_DataDirRow->lineEdit()->text() == defaultDataDirectory())
    {
        //ensure the dir is/gets made only when the default dir is still there
        if(!QFile::exists(dataDirectoryResolved()))
        {
            QDir defaultDir(dataDirectoryResolved());
            if(!defaultDir.mkpath(dataDirectoryResolved()))
            {
                QMessageBox::critical(this, tr("Error"), tr("Could not make directory: '") + dataDirectoryResolved() + "'");
                return;
            }
            else
            {
                qDebug() << "Made directory" << dataDirectoryResolved() << "for new user, since they didn't choose one explicitly";
            }
        }
    }
    //more profile properties can go here
    settings->endGroup();
    settings->endGroup();
    accept();
}
