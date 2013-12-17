#include "lossydirectorycopierwidget.h"

LossyDirectoryCopierWidget::LossyDirectoryCopierWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_SourceDirLabel = new QLabel("Source Folder:");
    m_SourceDirLineEdit = new QLineEdit("/run/shm/sourceTest1");
    m_DestDirLabel = new QLabel("Destination Folder (must be empty):");
    m_DestDirLineEdit = new QLineEdit("/run/shm/destTest1");
    m_StartButton = new QPushButton("Lossy Convert [with Copy Fallback]");
    m_StopButton = new QPushButton("Stop [after done with current item]");
    m_Debug = new QPlainTextEdit();

    QHBoxLayout *sourceRow = new QHBoxLayout();
    sourceRow->addWidget(m_SourceDirLabel);
    sourceRow->addWidget(m_SourceDirLineEdit);
    m_Layout->addLayout(sourceRow);

    QHBoxLayout *destRow = new QHBoxLayout();
    destRow->addWidget(m_DestDirLabel);
    destRow->addWidget(m_DestDirLineEdit);
    m_Layout->addLayout(destRow);

    m_Layout->addWidget(m_StartButton);
    m_Layout->addWidget(m_StopButton);
    m_Layout->addWidget(m_Debug);

    this->setLayout(m_Layout);

    connect(m_StartButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));
    connect(m_StopButton, SIGNAL(clicked()), this, SIGNAL(stopRequested()));
}
LossyDirectoryCopierWidget::~LossyDirectoryCopierWidget()
{

}
void LossyDirectoryCopierWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void LossyDirectoryCopierWidget::handleStartButtonClicked()
{
    if(m_SourceDirLineEdit->text().trimmed().isEmpty())
    {
        handleD("select a source dir");
        return;
    }
    if(m_DestDirLineEdit->text().trimmed().isEmpty())
    {
        handleD("select a destination dir");
        return;
    }
    emit lossilyCopyEveryDirectoryEntryAndJustCopyWheneverCantCompressRequested(m_SourceDirLineEdit->text(), m_DestDirLineEdit->text());
}
