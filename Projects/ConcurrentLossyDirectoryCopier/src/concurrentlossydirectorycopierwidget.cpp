#include "concurrentlossydirectorycopierwidget.h"

ConcurrentLossyDirectoryCopierWidget::ConcurrentLossyDirectoryCopierWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *theLayout = new QVBoxLayout();

    QHBoxLayout *sourceDirRow = new QHBoxLayout();
    QLabel *sourceDirLabel = new QLabel("Source Dir:");
    m_SourceDirLineEdit = new QLineEdit();
    sourceDirRow->addWidget(sourceDirLabel);
    sourceDirRow->addWidget(m_SourceDirLineEdit);
    theLayout->addLayout(sourceDirRow);

    QHBoxLayout *destDirRow = new QHBoxLayout();
    QLabel *destDirLabel = new QLabel("Destination Dir:");
    m_DestinationDirLineEdit = new QLineEdit();
    destDirRow->addWidget(destDirLabel);
    destDirRow->addWidget(m_DestinationDirLineEdit);
    theLayout->addLayout(destDirRow);

    QPushButton *startButton = new QPushButton("Concurrently Lossily Copy Directory");
    theLayout->addWidget(startButton);
    QPushButton *cancelButton = new QPushButton("Cancel");
    theLayout->addWidget(cancelButton);

    m_Debug = new QPlainTextEdit();
    theLayout->addWidget(m_Debug);

    setLayout(theLayout);

    connect(startButton, SIGNAL(clicked()), this, SLOT(handleStartButtonClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(cancelAfterAllCurrentlyEncodingVideosFinishRequested()));
}
ConcurrentLossyDirectoryCopierWidget::~ConcurrentLossyDirectoryCopierWidget()
{
}
void ConcurrentLossyDirectoryCopierWidget::recordUpdatedProgressMinsAndMaxes(int minProgress, int maxProgress)
{
    m_MinProgress = minProgress;
    m_MaxProgress = maxProgress;
    handleD(QString("New Min Progress: " + QString::number(minProgress)));
    handleD(QString("New Max Progress: " + QString::number(maxProgress)));
}
void ConcurrentLossyDirectoryCopierWidget::updateProgressValue(int newProgress)
{
    handleD(QString("Progress Now at: " + QString::number((((double)newProgress)/((double)m_MaxProgress-m_MinProgress))*100, 'f') + "% (" + QString::number(newProgress) + "/" + QString::number(m_MaxProgress-m_MinProgress) + ")"));
}
void ConcurrentLossyDirectoryCopierWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void ConcurrentLossyDirectoryCopierWidget::handleStartButtonClicked()
{
    if(m_SourceDirLineEdit->text().trimmed().isEmpty())
    {
        handleD("select source dir");
        return;
    }
    if(m_DestinationDirLineEdit->text().trimmed().isEmpty())
    {
        handleD("select destination dir");
        return;
    }
    emit lossilyCopyDirectoryUsingAllAvailableCoresRequested(m_SourceDirLineEdit->text(), m_DestinationDirLineEdit->text());
}
