#include "simplifiedheirarchymolesterwidget.h"

#include <QPushButton>

SimplifiedHeirarchyMolesterWidget::SimplifiedHeirarchyMolesterWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_DirectoryToMolestRow = new LabelLineEditBrowseButton("Directory To Molest:", LabelLineEditBrowseButton::OpenDirectoryMode);
    m_LastModifiedTimestampsFileRow = new LabelLineEditBrowseButton("Last Modified Timestamps File:");
    QPushButton *startButton = new QPushButton("Molest Heirarchy");
    m_Debug = new QPlainTextEdit();
    myLayout->addLayout(m_DirectoryToMolestRow);
    myLayout->addLayout(m_LastModifiedTimestampsFileRow);
    myLayout->addWidget(startButton);
    myLayout->addWidget(m_Debug);

    setLayout(myLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));
}
SimplifiedHeirarchyMolesterWidget::~SimplifiedHeirarchyMolesterWidget()
{

}
void SimplifiedHeirarchyMolesterWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void SimplifiedHeirarchyMolesterWidget::handleStartButtonClicked()
{
    if(m_DirectoryToMolestRow->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("select directory to molest");
        return;
    }
    if(m_LastModifiedTimestampsFileRow->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("select last modified timestamps file");
        return;
    }
    emit heirarchyMolestationRequested(m_DirectoryToMolestRow->lineEdit()->text(), m_LastModifiedTimestampsFileRow->lineEdit()->text());
}
