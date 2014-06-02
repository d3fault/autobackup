#include "lastmodifiedtimestampstoolswidget.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QMessageBox>

#include "labellineeditbrowsebutton.h"

LastModifiedTimestampsToolsWidget::LastModifiedTimestampsToolsWidget(QWidget *parent)
    : QWidget(parent)
    , m_Directory(new LabelLineEditBrowseButton("Directory:", LabelLineEditBrowseButton::OpenDirectoryMode))
    , m_LastModifiedTimestampsFileRow(new LabelLineEditBrowseButton("Last Modified Timestamps File:", LabelLineEditBrowseButton::SaveFileMode)) //TODOoptional: switch back and forth from save to open depending on mode. right now we'll just get silly overwrite question when trying to 'open'
    , m_GenerateLastModifiedTimestampsFileModeRadioButton(new QRadioButton("Generate Last Modified Timestamps File"))
    , m_StartButton(new QPushButton("Start"))
    , m_Debug(new QPlainTextEdit())
{
    QVBoxLayout *myLayout = new QVBoxLayout();

    QGroupBox *modeGroupBox = new QGroupBox();
    QRadioButton *molestHeirarchyUsingLastModifiedTimestampsFileModeRadioButton = new QRadioButton("Molest Heirarchy using Last Modified Timestamps File");
    m_GenerateLastModifiedTimestampsFileModeRadioButton->setChecked(true);
    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addWidget(m_GenerateLastModifiedTimestampsFileModeRadioButton);
    modeLayout->addWidget(molestHeirarchyUsingLastModifiedTimestampsFileModeRadioButton);
    modeGroupBox->setLayout(modeLayout);

    myLayout->addLayout(m_Directory);
    myLayout->addLayout(m_LastModifiedTimestampsFileRow);
    myLayout->addWidget(modeGroupBox);
    myLayout->addWidget(m_StartButton);
    myLayout->addWidget(m_Debug);

    setLayout(myLayout);

    connect(m_StartButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));

    handleD("WARNING: When used as a timestamps file GENERATOR, this app does not currently ignore .git folders and binaries etc like QuickDirty does");
}
void LastModifiedTimestampsToolsWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void LastModifiedTimestampsToolsWidget::reEnableStartButton()
{
    m_StartButton->setEnabled(true);
}
void LastModifiedTimestampsToolsWidget::handleStartButtonClicked()
{
    if(m_Directory->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("Select directory");
        return;
    }
    if(m_LastModifiedTimestampsFileRow->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("Select last modified timestamps file");
        return;
    }

    QMessageBox msgBox(QMessageBox::Question, "Are you sure?", "Are you sure you wish to continue? There is no going back. The process is irreversible. If you are drunk, press No");
    QPushButton *acceptButton = msgBox.addButton(tr("Yes"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("No"), QMessageBox::RejectRole);
    msgBox.exec();
    if(msgBox.clickedButton() == acceptButton)
    {
        m_StartButton->setEnabled(false);
        if(m_GenerateLastModifiedTimestampsFileModeRadioButton->isChecked())
        {
            emit lastModifiedTimestampsFileGenerationRequested(m_Directory->lineEdit()->text(), m_LastModifiedTimestampsFileRow->lineEdit()->text());
        }
        else
        {
            emit heirarchyMolestationRequested(m_Directory->lineEdit()->text(), m_LastModifiedTimestampsFileRow->lineEdit()->text());
        }
    }
}
