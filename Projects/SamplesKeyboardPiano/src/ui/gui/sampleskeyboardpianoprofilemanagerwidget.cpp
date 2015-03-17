#include "sampleskeyboardpianoprofilemanagerwidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>

SamplesKeyboardPianoProfileManagerWidget::SamplesKeyboardPianoProfileManagerWidget(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QHBoxLayout *addProfileRow = new QHBoxLayout();
    m_NewProfileNameLineEdit = new QLineEdit();
    m_NewProfileNameLineEdit->setFocus();
    QPushButton *addProfileButton = new QPushButton(tr("&Add Profile"));
    addProfileRow->addWidget(m_NewProfileNameLineEdit);
    addProfileRow->addWidget(addProfileButton);

    m_ProfilesComboBox = new QComboBox();

    QHBoxLayout *cancelOkRow = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    m_OkButton = new QPushButton(tr("&Ok"));
    m_OkButton->setDisabled(true);
    cancelOkRow->addWidget(cancelButton);
    cancelOkRow->addWidget(m_OkButton);

    myLayout->addLayout(addProfileRow);
    myLayout->addWidget(m_ProfilesComboBox);
    myLayout->addLayout(cancelOkRow);

    setLayout(myLayout);

    connect(addProfileButton, SIGNAL(clicked()), this, SLOT(handleAddProfileButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_OkButton, SIGNAL(clicked()), this, SLOT(accept()));
}
SamplesKeyboardPianoProfile SamplesKeyboardPianoProfileManagerWidget::selectedProfile() const
{
    return m_ProfilesComboBox->currentData().value<SamplesKeyboardPianoProfile>();
}
void SamplesKeyboardPianoProfileManagerWidget::handleProfilesProbed(QList<SamplesKeyboardPianoProfile> profiles)
{
    m_ProfilesComboBox->clear();
    Q_FOREACH(const SamplesKeyboardPianoProfile &currentProfile, profiles)
    {
        QVariant variant;
        variant.setValue(currentProfile);
        m_ProfilesComboBox->addItem(currentProfile.Name, variant);
        m_OkButton->setEnabled(true);
    }
    show();
}
void SamplesKeyboardPianoProfileManagerWidget::handleAddProfileButtonClicked()
{
    if(m_NewProfileNameLineEdit->text().trimmed().isEmpty())
        return;

    //TODOproper: emit addProfileRequested() -> backend -> addProfile -> profileAdded -> frontend/this -> handleProfileAdded
    //QSettings settings;

    SamplesKeyboardPianoProfile newProfile;
    newProfile.Name = m_NewProfileNameLineEdit->text();
    newProfile.BaseDir = "/home/d3fault/temp/Music/ccMixterCCYBY"; //TODOreq: browse line edit
    QVariant variant;
    variant.setValue(newProfile);
    m_ProfilesComboBox->addItem(newProfile.Name, variant); //TODOreq: serialize to QSettings xD
    m_OkButton->setEnabled(true);
}
