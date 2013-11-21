#include "gitunrollrerollcensorshipmachinewidget.h"

GitUnrollRerollCensorshipMachineWidget::GitUnrollRerollCensorshipMachineWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();

    m_FilePathToListOfFilepathsToCensorLabel = new QLabel("File with list of files to censor:");
    m_FilePathToListOfFilepathsToCensorLineEdit = new QLineEdit();
    m_FilePathToListOfFilepathsToCensorBrowseButton = new QPushButton("Browse");

    m_AbsoluteSourceGitDirToCensorLabel = new QLabel("Source Dir/Repo To Censor:");
    m_AbsoluteSourceGitDirToCensorLineEdit = new QLineEdit();
    m_AbsoluteSourceGitDirToCensorBrowseButton = new QPushButton("Browse");

    m_AbsoluteDestinationGitDirCensoredLabel = new QLabel("Destination for Censored Result/Repo:");
    m_AbsoluteDestinationGitDirCensoredLineEdit = new QLineEdit();
    m_AbsoluteDestinationGitDirCensoredBrowseButton = new QPushButton("Browse");

    m_AbsoluteWorkingDirLabel = new QLabel("Working Dir (use tmpfs):");
    m_AbsoluteWorkingDirLineEdit = new QLineEdit(QDir::tempPath());
    m_AbsoluteWorkingDirBrowseButton = new QPushButton("Browse");

    m_DoUnrollRerollButton = new QPushButton("Do Git Unroll/Reroll [Censoring Along The Way]");

    m_Debug = new QPlainTextEdit();


    QHBoxLayout *firstRow = new QHBoxLayout();
    firstRow->addWidget(m_FilePathToListOfFilepathsToCensorLabel);
    firstRow->addWidget(m_FilePathToListOfFilepathsToCensorLineEdit);
    firstRow->addWidget(m_FilePathToListOfFilepathsToCensorBrowseButton);
    m_Layout->addLayout(firstRow);

    QHBoxLayout *secondRow = new QHBoxLayout();
    secondRow->addWidget(m_AbsoluteSourceGitDirToCensorLabel);
    secondRow->addWidget(m_AbsoluteSourceGitDirToCensorLineEdit);
    secondRow->addWidget(m_AbsoluteSourceGitDirToCensorBrowseButton);
    m_Layout->addLayout(secondRow);

    QHBoxLayout *thirdRow = new QHBoxLayout();
    thirdRow->addWidget(m_AbsoluteDestinationGitDirCensoredLabel);
    thirdRow->addWidget(m_AbsoluteDestinationGitDirCensoredLineEdit);
    thirdRow->addWidget(m_AbsoluteDestinationGitDirCensoredBrowseButton);
    m_Layout->addLayout(thirdRow);

    QHBoxLayout *fourthRow = new QHBoxLayout();
    fourthRow->addWidget(m_AbsoluteWorkingDirLabel);
    fourthRow->addWidget(m_AbsoluteWorkingDirLineEdit);
    fourthRow->addWidget(m_AbsoluteWorkingDirBrowseButton);
    m_Layout->addLayout(fourthRow);

    m_Layout->addWidget(m_DoUnrollRerollButton);
    m_Layout->addWidget(m_Debug);

    setLayout(m_Layout);


    connect(m_FilePathToListOfFilepathsToCensorBrowseButton, SIGNAL(clicked()), this, SLOT(handleFilePathToListOfFilepathsToCensorBrowseButtonClicked()));
    connect(m_AbsoluteSourceGitDirToCensorBrowseButton, SIGNAL(clicked()), this, SLOT(handleAbsoluteSourceGitDirToCensorBrowseButtonClicked()));
    connect(m_AbsoluteDestinationGitDirCensoredBrowseButton, SIGNAL(clicked()), this, SLOT(handleAbsoluteDestinationGitDirCensoredBrowseButtonClicked()));
    connect(m_AbsoluteWorkingDirBrowseButton, SIGNAL(clicked()), this, SLOT(handleAbsoluteWorkingDirBrowseButtonClicked()));
    connect(m_DoUnrollRerollButton, SIGNAL(clicked()), this, SLOT(handleDoUnrollRerollButtonClicked()));
}
void GitUnrollRerollCensorshipMachineWidget::handleFilePathToListOfFilepathsToCensorBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select File Containing List Of File Paths [In Easy Tree Format]"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_FilePathToListOfFilepathsToCensorLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void GitUnrollRerollCensorshipMachineWidget::handleAbsoluteSourceGitDirToCensorBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Source Directory/Repo To Unroll/Re-Roll/Censor"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_AbsoluteSourceGitDirToCensorLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void GitUnrollRerollCensorshipMachineWidget::handleAbsoluteDestinationGitDirCensoredBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Choose Folder Name (MUST NOT EXIST) For Output/Censored Repo/Dir"));
    aFileDialog.setFileMode(QFileDialog::AnyFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptSave);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_AbsoluteDestinationGitDirCensoredLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void GitUnrollRerollCensorshipMachineWidget::handleAbsoluteWorkingDirBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Working Dir (tmpfs recommended)"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_AbsoluteWorkingDirLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void GitUnrollRerollCensorshipMachineWidget::handleDoUnrollRerollButtonClicked()
{
    if(m_FilePathToListOfFilepathsToCensorLineEdit->text().trimmed().isEmpty())
    {
        handleD("Specify File Path To List Of Filepaths To Censor");
        return;
    }
    if(m_AbsoluteSourceGitDirToCensorLineEdit->text().trimmed().isEmpty())
    {
        handleD("Specify Source Git Dir To Censor");
        return;
    }
    if(m_AbsoluteDestinationGitDirCensoredLineEdit->text().trimmed().isEmpty())
    {
        handleD("Specify Censored Destination/Output Dir/Repo (MUST NOT EXIST)");
        return;
    }
    if(m_AbsoluteWorkingDirLineEdit->text().trimmed().isEmpty())
    {
        handleD("Specify working dir (tmpfs recommended)");
        return;
    }

    emit unrollRerollGitRepoCensoringAtEachCommitRequested(m_FilePathToListOfFilepathsToCensorLineEdit->text(), m_AbsoluteSourceGitDirToCensorLineEdit->text(), m_AbsoluteDestinationGitDirCensoredLineEdit->text(), m_AbsoluteWorkingDirLineEdit->text());
}
void GitUnrollRerollCensorshipMachineWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
