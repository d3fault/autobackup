#include "mainwidget.h"

#include <QHBoxLayout>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    //Begin Layout
    m_Layout = new QVBoxLayout();

    QHBoxLayout *dirToDetectSymbolicLinksInRow = new QHBoxLayout();
    m_DirToDetectSymbolicLinksInLabel = new QLabel(tr("Dir To Look For Symbolic Links In:"));
    m_DirToDetectSymbolicLinksInLineEdit = new QLineEdit();
    m_DirToDetectSymbolicLinksInBrowseButton = new QPushButton(tr("Browse"));
    dirToDetectSymbolicLinksInRow->addWidget(m_DirToDetectSymbolicLinksInLabel);
    dirToDetectSymbolicLinksInRow->addWidget(m_DirToDetectSymbolicLinksInLineEdit);
    dirToDetectSymbolicLinksInRow->addWidget(m_DirToDetectSymbolicLinksInBrowseButton);

    QHBoxLayout *watDoWhenFoundRow = new QHBoxLayout();
    m_WhatToDoWhenSymbolicLinkDetectedLabel = new QLabel(tr("What to do when symbolic link found:"));
    m_QuitOnFirstDetectRadioButton = new QRadioButton(tr("Report First Symbolic Link Detected and Quit"));
    m_ListAllPathsRadioButton = new QRadioButton(tr("List All Symbolic Links Detected"));
    watDoWhenFoundRow->addWidget(m_WhatToDoWhenSymbolicLinkDetectedLabel);
    watDoWhenFoundRow->addWidget(m_QuitOnFirstDetectRadioButton);
    watDoWhenFoundRow->addWidget(m_ListAllPathsRadioButton);

    m_StartDetectingSymbolicLinksButton = new QPushButton(tr("Start Detecting Symbolic Links"));

    m_Debug = new QPlainTextEdit();

    m_Layout->addLayout(dirToDetectSymbolicLinksInRow);
    m_Layout->addLayout(watDoWhenFoundRow);
    m_Layout->addWidget(m_StartDetectingSymbolicLinksButton);
    m_Layout->addWidget(m_Debug);

    setLayout(m_Layout);
    //End Layout

    m_QuitOnFirstDetectRadioButton->setChecked(true);

    //Begin Connections
    //connect(m_QuitOnFirstDetectRadioButton, SIGNAL(toggled(bool)), this, SLOT(figureOutRadioBool()));
    //since there's only two we don't need to connect to both. If there was more than two, then the logic would get complicated and we'd need to connect to all of them! (OR SOMETHING. perhaps there's another solution idfk. maybe combobox is easier anyways gah)
    //fuck it ima figure it out after the 'start detecting' buttonis clicked lolol

    connect(m_DirToDetectSymbolicLinksInBrowseButton, SIGNAL(clicked()), this, SLOT(handleDirToDetectSymbolicLinksInBrowseButtonClicked()));
    connect(m_StartDetectingSymbolicLinksButton, SIGNAL(clicked()), this, SLOT(handleStartDetectingSymbolicLinksButtonClicked()));
    //End Connections
}
MainWidget::~MainWidget()
{   
}
void MainWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void MainWidget::handleDirToDetectSymbolicLinksInBrowseButtonClicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    dialog.setOption(QFileDialog::ReadOnly, true);
    dialog.setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    dialog.setWindowModality(Qt::WindowModal);

    if(dialog.exec() && dialog.selectedFiles().size() > 0)
    {
        m_DirToDetectSymbolicLinksInLineEdit->setText(dialog.selectedFiles().at(0));
    }
}
void MainWidget::handleStartDetectingSymbolicLinksButtonClicked()
{
    if(m_DirToDetectSymbolicLinksInLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select A Dir");
        return;
    }

    emit startDetectingSymbolicLinksRequested(m_DirToDetectSymbolicLinksInLineEdit->text(), m_QuitOnFirstDetectRadioButton->isChecked());
}
