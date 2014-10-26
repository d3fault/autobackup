#include "osioscreateprofiledialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QFileInfo>

#include "labellineeditbrowsebutton.h"

OsiosCreateProfileDialog::OsiosCreateProfileDialog(QWidget *parent)
    : QDialog(parent)
    , m_ProfileNameLineEdit(new QLineEdit())
    , m_DataDirRow(new LabelLineEditBrowseButton(tr("Data Dir:"), LabelLineEditBrowseButton::OpenDirectoryMode))
{
    {
        QSettings settings;
        QFileInfo settingsFileInfo(settings.fileName());
        m_DataDirRow->lineEdit()->setText(settingsFileInfo.absolutePath());
    }

    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *profileNameRow = new QHBoxLayout();
    profileNameRow->addWidget(new QLabel(tr("Profile Name:")));
    profileNameRow->addWidget(m_ProfileNameLineEdit);

    QHBoxLayout *cancelOkRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Create Profile"));
    cancelOkRow->addWidget(cancelButton);
    cancelOkRow->addWidget(okButton);

    myLayout->addLayout(profileNameRow);
    myLayout->addLayout(m_DataDirRow);
    myLayout->addLayout(cancelOkRow);
    setLayout(myLayout);

    okButton->setDefault(true);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}
QString OsiosCreateProfileDialog::newProfileName() const
{
    return m_ProfileNameLineEdit->text();
}
QString OsiosCreateProfileDialog::chosenDataDir() const
{
    return m_DataDirRow->lineEdit()->text();
}
