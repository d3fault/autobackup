#include "bulkfiletextprependerwidget.h"

BulkFileTextPrependerWidget::BulkFileTextPrependerWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *myLayout = new QVBoxLayout();
    m_DirSelectRow = new LabelLineEditBrowseButton("Directory:", LabelLineEditBrowseButton::OpenDirectoryMode);
    m_FileWithTextToPrepend = new LabelLineEditBrowseButton("File With Text To Prepend:");
    m_FilenameExtensionsLineEdit = new QLineEdit();
    QPushButton *startButton = new QPushButton("Start Prepending Text To All Files (Recursively) With Extensions");
    m_Debug = new QPlainTextEdit();

    myLayout->addLayout(m_DirSelectRow);
    myLayout->addLayout(m_FileWithTextToPrepend);
    QHBoxLayout *filenameExtensionsRow = new QHBoxLayout();
    filenameExtensionsRow->addWidget(new QLabel("Filename Extensions To Prepend Text To:"));
    filenameExtensionsRow->addWidget(m_FilenameExtensionsLineEdit);
    myLayout->addLayout(filenameExtensionsRow);
    myLayout->addWidget(startButton);
    myLayout->addWidget(m_Debug);
    setLayout(myLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));
}
BulkFileTextPrependerWidget::~BulkFileTextPrependerWidget()
{ }
void BulkFileTextPrependerWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void BulkFileTextPrependerWidget::handleStartButtonClicked()
{
    if(m_DirSelectRow->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("select a dir");
        return;
    }
    if(m_FileWithTextToPrepend->lineEdit()->text().trimmed().isEmpty())
    {
        handleD("select a file containing text to prepend");
        return;
    }
    QStringList extensionsStringList;
    if(!m_FilenameExtensionsLineEdit->text().trimmed().isEmpty())
    {
        extensionsStringList = m_FilenameExtensionsLineEdit->text().split(",", QString::SkipEmptyParts);
    }
    emit prependStringToBeginningOfAllTextFilesInDirRequested(m_DirSelectRow->lineEdit()->text(), m_FileWithTextToPrepend->lineEdit()->text(), extensionsStringList);
}
