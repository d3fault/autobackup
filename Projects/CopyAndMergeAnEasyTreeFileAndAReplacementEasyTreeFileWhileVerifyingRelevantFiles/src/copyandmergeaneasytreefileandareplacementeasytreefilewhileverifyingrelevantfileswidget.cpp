#include "copyandmergeaneasytreefileandareplacementeasytreefilewhileverifyingrelevantfileswidget.h"

CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    QHBoxLayout *regularEasyTreeFileRow = new QHBoxLayout();
    QLabel *regularEasyTreeFileLabel = new QLabel("Regular Easy Tree File:");
    m_RegularEasyTreeFileLineEdit = new QLineEdit();
    QPushButton *regularEasyTreeFileBrowseButton = new QPushButton("Browse");
    regularEasyTreeFileRow->addWidget(regularEasyTreeFileLabel);
    regularEasyTreeFileRow->addWidget(m_RegularEasyTreeFileLineEdit);
    regularEasyTreeFileRow->addWidget(regularEasyTreeFileBrowseButton);
    layout->addLayout(regularEasyTreeFileRow);
    connect(regularEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleRegularEasyTreeFileBrowseButtonClicked()));

    QHBoxLayout *replacementEasyTreeFileRow = new QHBoxLayout();
    QLabel *replacementEasyTreeFileLabel = new QLabel("Replacements Easy Tree File:");
    m_ReplacementEasyTreeFileLineEdit = new QLineEdit();
    QPushButton *replacementEasyTreeFileBrowseButton = new QPushButton("Browse");
    replacementEasyTreeFileRow->addWidget(replacementEasyTreeFileLabel);
    replacementEasyTreeFileRow->addWidget(m_ReplacementEasyTreeFileLineEdit);
    replacementEasyTreeFileRow->addWidget(replacementEasyTreeFileBrowseButton);
    layout->addLayout(replacementEasyTreeFileRow);
    connect(replacementEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleReplacementsEasyTreeFileBrowseButtonClicked()));

    QHBoxLayout *inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileRow = new QHBoxLayout();
    QLabel *inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLabel = new QLabel("Source Base Dir:");
    m_SourceBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLineEdit = new QLineEdit();
    QPushButton *inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileBrowseButton = new QPushButton("Browse");
    inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileRow->addWidget(inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLabel);
    inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileRow->addWidget(m_SourceBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLineEdit);
    inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileRow->addWidget(inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileBrowseButton);
    layout->addLayout(inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileRow);
    connect(inputBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleInputBaseDirCorrespondingToRegularEasyTreeFilesEasyTreeFileBrowseButtonClicked()));

    QHBoxLayout *outputDirToCopyToRow = new QHBoxLayout();
    QLabel *outputDirToCopyToLabel = new QLabel("Destination Dir:");
    m_DestinationDirToCopyToLineEdit = new QLineEdit();
    QPushButton *outputDirToCopyToBrowseButton = new QPushButton("Browse");
    outputDirToCopyToRow->addWidget(outputDirToCopyToLabel);
    outputDirToCopyToRow->addWidget(m_DestinationDirToCopyToLineEdit);
    outputDirToCopyToRow->addWidget(outputDirToCopyToBrowseButton);
    layout->addLayout(outputDirToCopyToRow);
    connect(outputDirToCopyToBrowseButton, SIGNAL(clicked()), this, SLOT(handleOutputDirToCopyToBrowseButtonClicked()));

    QHBoxLayout *outputEasyTreeFileRow = new QHBoxLayout();
    QLabel *outputEasyTreeFileLabel = new QLabel("Output Easy Tree File:");
    m_OutputEasyTreeFileLineEdit = new QLineEdit();
    QPushButton *outputEasyTreeFileBrowseButton = new QPushButton("Browse");
    outputEasyTreeFileRow->addWidget(outputEasyTreeFileLabel);
    outputEasyTreeFileRow->addWidget(m_OutputEasyTreeFileLineEdit);
    outputEasyTreeFileRow->addWidget(outputEasyTreeFileBrowseButton);
    layout->addLayout(outputEasyTreeFileRow);
    connect(outputEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleOutputEasyTreeFileBrowseButtonClicked()));

    QPushButton *copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesButton = new QPushButton("Copy And Merge An Easy Tree File And A Replacement Easy Tree File While Verifying Relevant Files");
    layout->addWidget(copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesButton);
    connect(copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesButton, SIGNAL(clicked()), this, SLOT(handleCopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesButtonClicked()));

    m_Debug = new QPlainTextEdit();
    layout->addWidget(m_Debug);

    m_Debug->appendPlainText("Make sure your replacements file does not contain any dir entries as those currently lead to undefined results");

    setLayout(layout);
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleCopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesButtonClicked()
{
    if(m_RegularEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select A Regular Easy Tree File");
        return;
    }
    if(m_ReplacementEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select A Replacement Easy Tree File");
        return; //TODOoptional: we could make it function without a replacement file, as that appears to be the common use case anyways
    }
    if(m_SourceBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select A Source Base Dir Corresponding To Easy Tree File");
        return;
    }
    if(m_DestinationDirToCopyToLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select A Destination Dir To Copy To");
        return;
    }
    if(m_OutputEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD("Select An Output Easy Tree File");
        return;
    }

    emit copyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesRequested(m_RegularEasyTreeFileLineEdit->text(), m_ReplacementEasyTreeFileLineEdit->text(), m_SourceBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLineEdit->text(), m_DestinationDirToCopyToLineEdit->text(), m_OutputEasyTreeFileLineEdit->text());
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleRegularEasyTreeFileBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Regular Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_RegularEasyTreeFileLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleReplacementsEasyTreeFileBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Replacements Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_ReplacementEasyTreeFileLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleInputBaseDirCorrespondingToRegularEasyTreeFilesEasyTreeFileBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Source Base Dir Corresponding to Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setOption(QFileDialog::ReadOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_SourceBaseDirCorrespondingToRegularEasyTreeFileEasyTreeFileLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleOutputDirToCopyToBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Destination Dir To Copy Files To"));
    aFileDialog.setFileMode(QFileDialog::Directory);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    aFileDialog.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_DestinationDirToCopyToLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
void CopyAndMergeAnEasyTreeFileAndAReplacementEasyTreeFileWhileVerifyingRelevantFilesWidget::handleOutputEasyTreeFileBrowseButtonClicked()
{
    QFileDialog aFileDialog(this, tr("Select Output Easy Tree File"));
    aFileDialog.setFileMode(QFileDialog::AnyFile);
    aFileDialog.setViewMode(QFileDialog::List);
    aFileDialog.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden);
    aFileDialog.setWindowModality(Qt::WindowModal);
    aFileDialog.setAcceptMode(QFileDialog::AcceptSave);

    if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
    {
        m_OutputEasyTreeFileLineEdit->setText(aFileDialog.selectedFiles().at(0));
    }
}
