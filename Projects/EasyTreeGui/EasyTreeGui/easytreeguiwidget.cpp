#include "easytreeguiwidget.h"

#include <QHBoxLayout>

EasyTreeGuiWidget::EasyTreeGuiWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Easy Tree Gui");

    m_Layout = new QVBoxLayout();

    QHBoxLayout *easyTreeDirHBoxLayout = new QHBoxLayout();
    m_DirToGenerateEasyTreeForLabel = new QLabel("Dir To Tree:");
    m_DirToGenerateEasyTreeForLineEdit = new QLineEdit();
    m_DirToGenerateEasyTreeForBrowseButton = new QPushButton("Browse");
    easyTreeDirHBoxLayout->addWidget(m_DirToGenerateEasyTreeForLabel, 0);
    easyTreeDirHBoxLayout->addWidget(m_DirToGenerateEasyTreeForLineEdit, 1);
    easyTreeDirHBoxLayout->addWidget(m_DirToGenerateEasyTreeForBrowseButton, 0);
    m_Layout->addLayout(easyTreeDirHBoxLayout);

    QHBoxLayout *easyTreeOutputFileHBoxLayout = new QHBoxLayout();
    m_EasyTreeOutputFilePathLabel = new QLabel("Tree Output File:");
    m_EasyTreeOutputFilePathLineEdit = new QLineEdit();
    m_EasyTreeOutputFilePathBrowseButton = new QPushButton("Browse");
    easyTreeOutputFileHBoxLayout->addWidget(m_EasyTreeOutputFilePathLabel, 0);
    easyTreeOutputFileHBoxLayout->addWidget(m_EasyTreeOutputFilePathLineEdit, 1);
    easyTreeOutputFileHBoxLayout->addWidget(m_EasyTreeOutputFilePathBrowseButton, 0);
    m_Layout->addLayout(easyTreeOutputFileHBoxLayout);

    m_GenerateEasyTreeFileButton = new QPushButton("Generate Tree File");
    m_Layout->addWidget(m_GenerateEasyTreeFileButton);

    m_DebugOutput = new QPlainTextEdit();
    m_Layout->addWidget(m_DebugOutput);

    setLayout(m_Layout);

    handleD("Warning, placing the output file in the dir to tree will cause the output file to be tree'd... on next run. This particular usage of EasyTree is not set up to filter out that filename (though you can edit the source to do it pretty easily");

    connect(m_DirToGenerateEasyTreeForBrowseButton, SIGNAL(clicked()), this, SLOT(chooseDirToTree()));
    connect(m_EasyTreeOutputFilePathBrowseButton, SIGNAL(clicked()), this, SLOT(chooseTreeOutputFilePath()));
    connect(m_GenerateEasyTreeFileButton, SIGNAL(clicked()), this, SLOT(doGenerateEasyTreeFile()));
}
void EasyTreeGuiWidget::handleD(const QString &msg)
{
    m_DebugOutput->appendPlainText(msg);
}
void EasyTreeGuiWidget::chooseDirToTree()
{
    QFileDialog aFileDialog(this, tr("Select Dir To Tree"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_DirToGenerateEasyTreeForLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void EasyTreeGuiWidget::chooseTreeOutputFilePath()
{
    QFileDialog aFileDialog(this, tr("Save Output Tree File Where?"));
    aFileDialog.setFileMode(QFileDialog::AnyFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setOption(QFileDialog::DontConfirmOverwrite, false);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_EasyTreeOutputFilePathLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void EasyTreeGuiWidget::doGenerateEasyTreeFile()
{
    if(m_DirToGenerateEasyTreeForLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select a dir to tree");
        return;
    }
    if(m_EasyTreeOutputFilePathLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select a file for tree output");
        return;
    }
    handleD("gui is requesting that a tree file is generated");
    emit generateEasyTreeFileRequested(m_DirToGenerateEasyTreeForLineEdit->text(), m_EasyTreeOutputFilePathLineEdit->text());
}
void EasyTreeGuiWidget::handleEasyTreeGuiBusinessFinishedGeneratingEasyTreeFile()
{
    handleD("gui has received notice from business that the operation is complete");
}
