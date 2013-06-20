#include "lifeshaperstaticfileswidget.h"

LifeShaperStaticFilesWidget::LifeShaperStaticFilesWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();

    QHBoxLayout *inputEasyTreeFileRow = new QHBoxLayout();
    inputEasyTreeFileRow->addWidget(new QLabel(tr("Input Easy Tree Hash File:")));
    m_InputEasyTreeFileLineEdit = new QLineEdit();
    inputEasyTreeFileRow->addWidget(m_InputEasyTreeFileLineEdit, 1);
    QPushButton *inputEasyTreeFileBrowseButton = new QPushButton(tr("Browse"));
    inputEasyTreeFileRow->addWidget(inputEasyTreeFileBrowseButton);

    QHBoxLayout *lineLeftOffOnLastRunRow = new QHBoxLayout();
    lineLeftOffOnLastRunRow->addWidget(new QLabel(tr("Line Last Left Off On:")));
    m_LineLeftOffOnLastRunLineEdit = new QLineEdit();
    lineLeftOffOnLastRunRow->addWidget(m_LineLeftOffOnLastRunLineEdit, 1);

    QHBoxLayout *inputDirThatEasyTreeFileSpecifiesRow = new QHBoxLayout();
    inputDirThatEasyTreeFileSpecifiesRow->addWidget(new QLabel(tr("Input Dir Corresponding To Easy Tree Hash File:")));
    m_InputDirThatEasyTreeFileSpecifiesLineEdit = new QLineEdit();
    inputDirThatEasyTreeFileSpecifiesRow->addWidget(m_InputDirThatEasyTreeFileSpecifiesLineEdit, 1);
    QPushButton *inputDirThatEasyTreeFileSpecifiesBrowseButton = new QPushButton(tr("Browse"));
    inputDirThatEasyTreeFileSpecifiesRow->addWidget(inputDirThatEasyTreeFileSpecifiesBrowseButton);

    QHBoxLayout *outputFilesDirRow = new QHBoxLayout();
    outputFilesDirRow->addWidget(new QLabel(tr("Output Folder To Put Files In:")));
    m_OutputFilesDirLineEdit = new QLineEdit();
    outputFilesDirRow->addWidget(m_OutputFilesDirLineEdit, 1);
    QPushButton *outputFilesDirBrowseButton = new QPushButton(tr("Browse"));
    outputFilesDirRow->addWidget(outputFilesDirBrowseButton);    

    m_StartIteratingEasyTreeHashFile = new QPushButton(tr("Start Iterating Easy Tree Hash File [from where we left off]"));
    m_StopIteratingEasyTreeHashFile = new QPushButton(tr("Stop Iterating"));

    m_CurrentEasyTreeHashLineLineEdit = new QLineEdit();

    //Analysis
    QHBoxLayout *currentFilePathRow = new QHBoxLayout();
    m_RelativeFilePathLabel = new QLabel();
    QFont boldFont = m_RelativeFilePathLabel->font();
    boldFont.setBold(true);
    m_RelativeFilePathLabel->setFont(boldFont);
    currentFilePathRow->addWidget(m_RelativeFilePathLabel, 1);

    QHBoxLayout *analysisButtonsRow = new QHBoxLayout();
    m_CopyAbsolutePathToClipboard = new QPushButton("Copy Absolute Path To Clipboard");
    analysisButtonsRow->addWidget(m_CopyAbsolutePathToClipboard);
    QPushButton *openWithMousepadButton = new QPushButton("Open In Mousepad");
    analysisButtonsRow->addWidget(openWithMousepadButton);
    QPushButton *launchWithDefaultApplicationButton = new QPushButton("Launch With Default");
    analysisButtonsRow->addWidget(launchWithDefaultApplicationButton);
    //QPushButton *viewInTerminalButton = new QPushButton("View In Terminal");
    //analysisButtonsRow->addWidget(viewInTerminalButton);
    QPushButton *viewInWebBrowserButton = new QPushButton("View In Web Browser");
    analysisButtonsRow->addWidget(viewInWebBrowserButton);
    QPushButton *openWithMplayerButton = new QPushButton("mplayer");
    analysisButtonsRow->addWidget(openWithMplayerButton);


    //Attributes
    QHBoxLayout *currentFileAttribsRow = new QHBoxLayout();
    m_IsDirLabel = new QLabel();
    m_IsDirLabel->setMinimumSize(50, 50);
    m_FileSizeLabel = new QLabel();
    m_CreationDateTimeLabel = new QLabel();
    m_LastModifiedDateTimeLabel = new QLabel();
    currentFileAttribsRow->addWidget(m_IsDirLabel);
    currentFileAttribsRow->addWidget(m_FileSizeLabel);
    currentFileAttribsRow->addWidget(m_CreationDateTimeLabel);
    currentFileAttribsRow->addWidget(m_LastModifiedDateTimeLabel);

    m_ApplyToThisAndAllFollowingUntilParentDirReEntered = new QCheckBox(tr("Apply To This And All Following Until Parent Is Re-Entered ('Recursive from _here_')"));

    //Decision Buttons
    QHBoxLayout *mainDecisionsRow = new QHBoxLayout();
    m_LeaveBehindButton = new QPushButton(tr("Leave Behind"));
    m_IffyCopyrightButton = new QPushButton(tr("Iffy Copyright"));
    m_DeferDecicionButton = new QPushButton(tr("Defer Decision"));
    m_BringForwardButton = new QPushButton(tr("Bring Forward"));
    mainDecisionsRow->addWidget(m_LeaveBehindButton);
    mainDecisionsRow->addWidget(m_IffyCopyrightButton);
    mainDecisionsRow->addWidget(m_DeferDecicionButton);
    mainDecisionsRow->addWidget(m_BringForwardButton);

    QHBoxLayout *replacementDecisionRow = new QHBoxLayout();
    m_Use_THIS_AsReplacementButton = new QPushButton(tr("Use _THIS_ as Replacement:"));
    m_FilePathForTHISreplacement = new QLineEdit();
    replacementDecisionRow->addWidget(m_Use_THIS_AsReplacementButton);
    replacementDecisionRow->addWidget(m_FilePathForTHISreplacement, 1);
    m_Use_THIS_AsReplacementBrowseButton = new QPushButton(tr("Browse"));
    replacementDecisionRow->addWidget(m_Use_THIS_AsReplacementBrowseButton);

    m_Debug = new QPlainTextEdit();
    m_Debug->setMaximumBlockCount(2000); //so we don't run out of memory

    m_Layout->addLayout(inputEasyTreeFileRow);
    m_Layout->addLayout(lineLeftOffOnLastRunRow);
    m_Layout->addLayout(inputDirThatEasyTreeFileSpecifiesRow);
    m_Layout->addLayout(outputFilesDirRow);
    m_Layout->addWidget(m_StartIteratingEasyTreeHashFile);
    m_Layout->addWidget(m_StopIteratingEasyTreeHashFile);
    m_Layout->addWidget(m_CurrentEasyTreeHashLineLineEdit);
    m_Layout->addLayout(currentFilePathRow);
    m_Layout->addLayout(analysisButtonsRow);
    m_Layout->addLayout(currentFileAttribsRow);
    m_Layout->addWidget(m_ApplyToThisAndAllFollowingUntilParentDirReEntered);
    m_Layout->addLayout(mainDecisionsRow);
    m_Layout->addLayout(replacementDecisionRow);
    m_Layout->addWidget(m_Debug);
    setLayout(m_Layout);

    handleD(tr("Debug window has maximum line count of 2k just FYI"));
    handleD(tr("note #1: for a directory, 'use this as replacement' button has recursion implied. also, you cannot recursively 'use this as a replacement' for a file, because you can't supply more than one replacement in the GUI"));
    handleD("note #2: leave behind, iffy copyright, and defer decision are all implicitly recursive when the target is a directory");
    handleD(tr("note #3: when replacing a dir, you must select a dir to replace it as 'this' or else you will get undefined results"));
    handleD("note #4: be aware of the bug when trying to do a recursive decision on a file in the root directory. it will error out but the first entry will still be added. if you try to resume from that entry, it will be inserted as a duplicate");

    connect(inputEasyTreeFileBrowseButton, SIGNAL(clicked()), this, SLOT(handleInputEasyTreeFileBrowseButtonClicked()));
    connect(inputDirThatEasyTreeFileSpecifiesBrowseButton, SIGNAL(clicked()), this, SLOT(handleInputDirThatEasyTreeFileSpecifiesBrowseButtonClicked()));
    connect(outputFilesDirBrowseButton, SIGNAL(clicked()), this, SLOT(handleOutputFilesFolderBrowseButtonClicked()));

    connect(m_StartIteratingEasyTreeHashFile, SIGNAL(clicked()), this, SLOT(handleStartIteratingEasyTreeHashFileButtonClicked()));
    connect(m_StopIteratingEasyTreeHashFile, SIGNAL(clicked()), this, SLOT(handleStopIteratingEasyTreeHashFileButtonClicked()));

    connect(m_CopyAbsolutePathToClipboard, SIGNAL(clicked()), this, SLOT(handleCopyAbsolutePathToClipboardClicked()));

    connect(openWithMousepadButton, SIGNAL(clicked()), this, SLOT(handleOpenWithMousepadClicked()));
    connect(launchWithDefaultApplicationButton, SIGNAL(clicked()), this, SLOT(handleLaunchWithDefaultApplicationClicked()));
    //connect(viewInTerminalButton, SIGNAL(clicked()), this, SLOT(handleOpenDirectoryInTerminalClicked()));
    connect(viewInWebBrowserButton, SIGNAL(clicked()), this, SLOT(handleOpenInWebBrowserlClicked()));
    connect(openWithMplayerButton, SIGNAL(clicked()), this, SLOT(handleOpenInMplayerClicked()));

    connect(m_ApplyToThisAndAllFollowingUntilParentDirReEntered, SIGNAL(toggled(bool)), this, SIGNAL(recursionFromCurrentChanged(bool)));

    connect(m_LeaveBehindButton, SIGNAL(clicked()), this, SLOT(aDecisionButtonWasClicked()));
    connect(m_IffyCopyrightButton, SIGNAL(clicked()), this, SLOT(aDecisionButtonWasClicked()));
    connect(m_DeferDecicionButton, SIGNAL(clicked()), this, SLOT(aDecisionButtonWasClicked()));
    connect(m_BringForwardButton, SIGNAL(clicked()), this, SLOT(aDecisionButtonWasClicked()));
    connect(m_Use_THIS_AsReplacementButton, SIGNAL(clicked()), this, SLOT(aDecisionButtonWasClicked()));

    connect(m_Use_THIS_AsReplacementBrowseButton, SIGNAL(clicked()), this, SLOT(handleBrowseForReplacementButtonClicked()));

    connect(m_LeaveBehindButton, SIGNAL(clicked()), this, SIGNAL(leaveBehindDecided()));
    connect(m_IffyCopyrightButton, SIGNAL(clicked()), this, SIGNAL(iffyCopyrightDecided()));
    connect(m_DeferDecicionButton, SIGNAL(clicked()), this, SIGNAL(deferDecisionDecided()));
    connect(m_BringForwardButton, SIGNAL(clicked()), this, SIGNAL(bringForwardDecided()));
    connect(m_Use_THIS_AsReplacementButton, SIGNAL(clicked()), this, SLOT(handleUseTHISasReplacementClicked()));

    setGuiEnabledBasedOnWhetherStarted(false);
    setDecisionsButtansEnabled(false);
}
void LifeShaperStaticFilesWidget::setGuiEnabledBasedOnWhetherStarted(bool started)
{
    m_InputEasyTreeFileLineEdit->setEnabled(!started);

    m_InputDirThatEasyTreeFileSpecifiesLineEdit->setEnabled(!started);

    m_OutputFilesDirLineEdit->setEnabled(!started);

    m_LineLeftOffOnLastRunLineEdit->setEnabled(!started);

    m_StartIteratingEasyTreeHashFile->setEnabled(!started);
    m_StopIteratingEasyTreeHashFile->setEnabled(started);
}
void LifeShaperStaticFilesWidget::setDecisionsButtansEnabled(bool enabled)
{
    m_CopyAbsolutePathToClipboard->setEnabled(enabled);

    m_ApplyToThisAndAllFollowingUntilParentDirReEntered->setEnabled(enabled);

    m_LeaveBehindButton->setEnabled(enabled);
    m_IffyCopyrightButton->setEnabled(enabled);
    m_DeferDecicionButton->setEnabled(enabled);
    m_BringForwardButton->setEnabled(enabled);
    m_Use_THIS_AsReplacementButton->setEnabled(enabled);
    m_FilePathForTHISreplacement->setEnabled(enabled);
    m_Use_THIS_AsReplacementBrowseButton->setEnabled(enabled);
}
QString LifeShaperStaticFilesWidget::getFullPathOfCurrentItem()
{
    return (m_InputDirThatEasyTreeFileSpecifies + m_RelativeFilePathLabel->text());
}
void LifeShaperStaticFilesWidget::handleInputEasyTreeFileBrowseButtonClicked()
{
    QFileDialog fileDialog;
    fileDialog.setOption(QFileDialog::ReadOnly, true);
    fileDialog.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
    fileDialog.setWindowModality(Qt::WindowModal);

    if(fileDialog.exec() && fileDialog.selectedFiles().size() > 0)
    {
        m_InputEasyTreeFileLineEdit->setText(fileDialog.selectedFiles().at(0));
    }
}
void LifeShaperStaticFilesWidget::handleInputDirThatEasyTreeFileSpecifiesBrowseButtonClicked()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ReadOnly, true);
    fileDialog.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    fileDialog.setWindowModality(Qt::WindowModal);

    if(fileDialog.exec() && fileDialog.selectedFiles().size() > 0)
    {
        m_InputDirThatEasyTreeFileSpecifiesLineEdit->setText(fileDialog.selectedFiles().at(0));
    }
}
void LifeShaperStaticFilesWidget::handleOutputFilesFolderBrowseButtonClicked()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    fileDialog.setWindowModality(Qt::WindowModal);

    if(fileDialog.exec() && fileDialog.selectedFiles().size() > 0)
    {
        m_OutputFilesDirLineEdit->setText(fileDialog.selectedFiles().at(0));
    }
}
void LifeShaperStaticFilesWidget::handleStartIteratingEasyTreeHashFileButtonClicked()
{
    QString lineLeftOffFrom(""); //empty string if there isn't one, our backend can handle this

    if(m_InputEasyTreeFileLineEdit->text().trimmed().isEmpty())
    {
        handleD(tr("select input easy tree hash file"));
        return;
    }
    if(m_OutputFilesDirLineEdit->text().trimmed().isEmpty())
    {
        handleD("select folder for output files");
        return;
    }
    m_OutputFilesFolder = m_OutputFilesDirLineEdit->text();
    if(!m_OutputFilesFolder.endsWith("/"))
    {
        m_OutputFilesFolder.append("/");
    }
    if(!m_LineLeftOffOnLastRunLineEdit->text().trimmed().isEmpty())
    {
        lineLeftOffFrom.append(m_LineLeftOffOnLastRunLineEdit->text());
    }

    //TODOreq: use the corresponding input dir/files with analysis buttons etc
    if(m_InputDirThatEasyTreeFileSpecifiesLineEdit->text().trimmed().isEmpty())
    {
        handleD("select input dir corresponding to easy tree hash content");
        return;
    }
    m_InputDirThatEasyTreeFileSpecifies = m_InputDirThatEasyTreeFileSpecifiesLineEdit->text();
    if(!m_InputDirThatEasyTreeFileSpecifies.endsWith("/"))
    {
        m_InputDirThatEasyTreeFileSpecifies.append("/");
    }

    setGuiEnabledBasedOnWhetherStarted(true);
    emit startIteratingEasyTreeHashFileRequested(m_InputEasyTreeFileLineEdit->text(), m_OutputFilesFolder, lineLeftOffFrom);
}
void LifeShaperStaticFilesWidget::handleStopIteratingEasyTreeHashFileButtonClicked()
{
    setGuiEnabledBasedOnWhetherStarted(false);
    emit stopIteratingRequested();
}
void LifeShaperStaticFilesWidget::handleCopyAbsolutePathToClipboardClicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(getFullPathOfCurrentItem());
    handleD("copied to clipboard");
}
void LifeShaperStaticFilesWidget::handleOpenWithMousepadClicked()
{
    QStringList mousepadArgs;
    mousepadArgs.append(getFullPathOfCurrentItem());
    QProcess::startDetached("/usr/bin/mousepad", mousepadArgs);
}
void LifeShaperStaticFilesWidget::handleLaunchWithDefaultApplicationClicked()
{
    QDesktopServices::openUrl(QUrl(QString("file://") + getFullPathOfCurrentItem()));
}
#if 0
void LifeShaperStaticFilesWidget::handleOpenDirectoryInTerminalClicked()
{
    QStringList terminalArgs;
    //QString fullPath = getFullPathOfCurrentItem();
    //QString fullPathInQuotes = fullPath.endsWith("/") ? (QString("\"") + fullPath + QString("\"")) : (fullPath.contains("/") ? (fullPath.left(fullPath.length()-fullPath.lastIndexOf("/"))) : ((QString("\"") + m_InputDirThatEasyTreeFileSpecifies + (QString("\""))))); //real men
    terminalArgs.append(QString("--working-directory=") + getFullPathOfCurrentItem());
    QProcess::startDetached("/usr/bin/xfce4-terminal", terminalArgs);
}
#endif
void LifeShaperStaticFilesWidget::handleOpenInWebBrowserlClicked()
{
    QStringList iceWeaselArgs;
    iceWeaselArgs.append(getFullPathOfCurrentItem());
    QProcess::startDetached("/usr/bin/iceweasel", iceWeaselArgs);
}
void LifeShaperStaticFilesWidget::handleOpenInMplayerClicked()
{
    QStringList mplayerArgs;
    mplayerArgs.append(getFullPathOfCurrentItem());
    QProcess::startDetached("/usr/bin/mplayer", mplayerArgs);
}
void LifeShaperStaticFilesWidget::handleNowProcessingEasyTreeHashItem(QString easyTreeHashLine, QString relativeFilePath, bool isDirectory, qint64 fileSize_OR_zeroIfDir, QString creationDateTime, QString lastModifiedDateTime)
{
    m_CurrentEasyTreeHashLineLineEdit->setText(easyTreeHashLine);

    m_RelativeFilePathLabel->setText(relativeFilePath);

    m_CurrentItemIsDir = isDirectory;

    if(isDirectory)
    {
        m_IsDirLabel->setText(tr("Dir"));
        m_IsDirLabel->setStyleSheet("QLabel { background: green; }");

        m_FileSizeLabel->setText(tr("n/a"));
    }
    else
    {
        m_IsDirLabel->setText(tr("File"));
        m_IsDirLabel->setStyleSheet("QLabel { background: yellow; }");

        m_FileSizeLabel->setText(QString::number(fileSize_OR_zeroIfDir));
    }

    m_CreationDateTimeLabel->setText(creationDateTime);
    m_LastModifiedDateTimeLabel->setText(lastModifiedDateTime);

    m_FilePathForTHISreplacement->clear();

    setDecisionsButtansEnabled(true);
}
void LifeShaperStaticFilesWidget::handleStoppedRecursingDatOperationBecauseParentDirEntered()
{
    m_ApplyToThisAndAllFollowingUntilParentDirReEntered->setChecked(false);
}
void LifeShaperStaticFilesWidget::handleBrowseForReplacementButtonClicked()
{
    QFileDialog fileDialog;
    fileDialog.setOption(QFileDialog::ReadOnly, true);
    //TODOoptional: custom filter based on whether current item is file or folder
    QDir::Filters filter = fileDialog.filter();
    filter |= QDir::NoDotAndDotDot;
    filter |= QDir::Hidden;
    if(m_CurrentItemIsDir)
    {
        fileDialog.setFileMode(QFileDialog::Directory);
        filter |= QDir::Dirs;
        filter |= QDir::Drives;
    }
    else
    {
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        filter |= QDir::Files;
    }
    fileDialog.setFilter(filter);
    fileDialog.setWindowModality(Qt::WindowModal);

    if(fileDialog.exec() && fileDialog.selectedFiles().size() > 0)
    {
        m_FilePathForTHISreplacement->setText(fileDialog.selectedFiles().at(0));
    }
}
void LifeShaperStaticFilesWidget::handleUseTHISasReplacementClicked()
{
    if(m_FilePathForTHISreplacement->text().trimmed().isEmpty())
    {
        handleD(tr("you didn't select a file to use as the replacement"));
        setDecisionsButtansEnabled(true);
        return;
    }
    emit useTHISasReplacementDecided(m_FilePathForTHISreplacement->text());
}
void LifeShaperStaticFilesWidget::aDecisionButtonWasClicked()
{
    setDecisionsButtansEnabled(false);
}
void LifeShaperStaticFilesWidget::handleFinishedProcessingEasyTreeHashFile()
{
    //fucking woot
    setGuiEnabledBasedOnWhetherStarted(false);
    handleD("gui received notice that business finished processing easy tree hash file");
}
void LifeShaperStaticFilesWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
