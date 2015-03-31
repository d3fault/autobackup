#include "nothingfancynotepadwithautofilenamebutmanualsavemainwindow.h"

#include <QAction>
#include <QToolBar>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCryptographicHash> //NVM(since i'm not storing milliseconds, i don't want to RISK 16 bits not being enough):git stores this, so we'll only use a crc32 instead
#include <QDateTime>
#include <QStyle>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTabWidget>
#include <QTemporaryFile>
//#include <QBuffer>

#include "newprofiledialog.h"
#include "nothingfancyplaintextedit.h"

//TODOoptional: could be specifiable in the user profile
#define NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Prefix "minddump_"
#define NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Extension ".txt"

//TODOmb: until modifying/overwriting is implemented, perhaps set the text exitor to read-only after [successful] save? Should still be able to highlight text
//TODOreq: save as
//TODOreq: use first keystroke for timestamp (touch after write? in filename at very least (maybe no touch so we get 2x timestamps lewlies)) -- the main reason for this is to support another TODOreq: on close it should ask you if you want to save-all/cancel (a 3rd option is usually discard-all, but ehhhhhh). On such "save-all" uses, 10x (really 'n') documents would get identical timestamps (Buh Osios2 is so much better (keystrokes logged :-P)). Using "timestamp of first keystroke" is a hacky way around that to keep the ordering of the docs relatively in tact (but still not exact because shit i coulda switched tabs like crazy and editted sporatically ofc)
//TODOreq: use QTemporaryFile for atomic "create only if not exist" (let random chars be in it, fuck it). QSaveFile should only be used when modifying/overwriting (should I change the filename [that has the date/time in it] also????)
//TODOreq: color indication of whether or not a file (or perhaps 'tab') is saved
//TODOreq: auto save and manual save. the former when doing mind dump, the latter used when writing project designs and wanting good filenames
//TODOoptional: when a close attempt is detected, iterate all open docs and see if they need to be saved. show a dialog with each unsaved doc. each doc has a radio selection for "auto save" and "manual save". next to manual save and only enabled when relevant is a line edit (or browse button or both?)
NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_MainTabWidget(new QTabWidget())
    , m_CurrentVisibleTabIndex(1)
    , m_FolderizeBaseDir(true)
{
    setWindowTitle(tr("Nothing Fancy Notepad with Auto-Filename Save"));
    setCentralWidget(m_MainTabWidget);

    createActions();
    createToolbars();

    QSettings settings;
    restoreGeometry(settings.value("windowGeometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_CurrentProfile = settings.value("lastUsedProfile").toString();
    if(m_CurrentProfile.isEmpty())
    {
        //create profile
        NewProfileDialog newProfileDialog(this);
        if(newProfileDialog.exec() != QDialog::Accepted)
        {
            close();
        }
        m_CurrentProfile = newProfileDialog.newProfileName();
        m_CurrentProfileBaseDir_WithSlashAppended = newProfileDialog.newProfileBaseDir();
        m_FolderizeBaseDir = newProfileDialog.folderizeBaseDir();
        settings.setValue("lastUsedProfile", m_CurrentProfile);

        settings.beginGroup(m_CurrentProfile);
        settings.setValue("baseDir", m_CurrentProfileBaseDir_WithSlashAppended);
        settings.setValue("folderizeBaseDir", m_FolderizeBaseDir);
        settings.endGroup();
    }
    else //TODOreq: arg and/or file menu item to go to profile manager
    {
        //load profile
        settings.beginGroup(m_CurrentProfile);
        m_CurrentProfileBaseDir_WithSlashAppended = NewProfileDialog::appendSlashIfNeeded(settings.value("baseDir").toString());
        m_FolderizeBaseDir = settings.value("folderizeBaseDir").toBool();
        settings.endGroup();
    }

    addNewFileTabAndMakeItCurrent();

    connect(m_MainTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleMainTabWidgetCurrentTabChanged(int)));
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::closeEvent(QCloseEvent *closeEvent)
{
    QSettings settings;
    settings.setValue("windowGeometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(closeEvent);
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::createActions()
{
    m_NewFileTabAction = new QAction(tr("New File"), this);
    m_NewFileTabAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));

    m_AutoFilenameSaveAction = new QAction(tr("Auto-Filename Save"), this);
    m_AutoFilenameSaveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_AutoFilenameSaveAction->setDisabled(true); //initially nothing to save

    connect(m_NewFileTabAction, SIGNAL(triggered()), this, SLOT(handleNewFileTabActionTriggered()));
    connect(m_AutoFilenameSaveAction, SIGNAL(triggered()), this, SLOT(handleAutoFilenameSaveActionTriggered()));
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::createToolbars()
{
    QToolBar *documentOperationsToolbar = addToolBar(tr("Document Operations Toolbar"));
    documentOperationsToolbar->setObjectName("documentOperationsToolbar");

    documentOperationsToolbar->addAction(m_NewFileTabAction);
    documentOperationsToolbar->addAction(m_AutoFilenameSaveAction);
}
bool NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::filePathTemplateFromCurrentDateTime(QString *out_FilePathTemplate)
{
    QString fileName = NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Prefix + QString::number(QDateTime::currentMSecsSinceEpoch() / static_cast<qint64>(1000));
    //QByteArray contentHashHex = myHexHash(fileContents);
    //fileName.append(contentHashHex);
    fileName.append("_XXXXXX"); //omg wat if the content hash has 6 Xs omg mind = blown (if these 6 Xs are still Xs, we KNOW that 6 chars of the hash are incorrect (and since we know the file contents, we can recalc them lol (but blah this idea sucks because why even store the hash in the filename them if we "can recalc it")). bitcoin proves that there can be series of same characters
    fileName.append(NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Extension);
    if(m_FolderizeBaseDir)
    {
        QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
        QString folderizedPath = m_CurrentProfileBaseDir_WithSlashAppended
                                + QString::number(currentDateTime.date().year()) //2014
                                + QDir::separator()
                                + currentDateTime.date().toString("MM-MMMM") //01-January
                                + QDir::separator()
                                ; //+ currentDateTime.date().toString("dd")
                                //+ QDir::separator();

        QFileInfo folderizedPathInfo(folderizedPath);
        bool exists = folderizedPathInfo.exists();
        if(exists)
        {
            if(!folderizedPathInfo.isDir())
            {
                QMessageBox::critical(this, tr("Error"), tr("Expected a folder, but it isn't a folder: '") + folderizedPath + tr("'"));
                return false;
            }
        }
        else
        {
            QDir folderizedDir(folderizedPath);
            if(!folderizedDir.mkpath(folderizedPath))
            {
                QMessageBox::critical(this, tr("Error Creating Directories"), tr("Failed to mkpath '") + folderizedPath + tr("'"));
                return false;
            }
        }
        (*out_FilePathTemplate) = QString(folderizedPath + fileName);
    }
    else
    {
        (*out_FilePathTemplate) = QString(m_CurrentProfileBaseDir_WithSlashAppended + fileName);
    }
    return true;
}
QByteArray NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::myHexHash(const QByteArray &inputByteArray)
{
    QByteArray contentHash = QCryptographicHash::hash(inputByteArray, QCryptographicHash::Md5);
    QByteArray contentHashHex = contentHash.toHex();
    return contentHashHex;
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::addNewFileTabAndMakeItCurrent()
{
    NothingFancyPlainTextEdit *newTextEdit = new NothingFancyPlainTextEdit();
    int newTabIndex = m_MainTabWidget->addTab(newTextEdit, QString::number(m_CurrentVisibleTabIndex));
    m_MainTabWidget->setCurrentIndex(newTabIndex);
    newTextEdit->setFocus();
    ++m_CurrentVisibleTabIndex;

    connect(newTextEdit, SIGNAL(textChanged()), this, SLOT(handleMainTextEditChanged()));
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::handleMainTabWidgetCurrentTabChanged(int newIndex)
{
    NothingFancyPlainTextEdit *theTextEditTab = static_cast<NothingFancyPlainTextEdit*>(m_MainTabWidget->widget(newIndex));
    m_AutoFilenameSaveAction->setEnabled(theTextEditTab->textHasBeenEditted());
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::handleMainTextEditChanged()
{
    if(!m_AutoFilenameSaveAction->isEnabled())
    {
        m_AutoFilenameSaveAction->setEnabled(true);
    }
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::handleNewFileTabActionTriggered()
{
    addNewFileTabAndMakeItCurrent();
}
void NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow::handleAutoFilenameSaveActionTriggered()
{
    NothingFancyPlainTextEdit *theTextEditTab = static_cast<NothingFancyPlainTextEdit*>(m_MainTabWidget->currentWidget());

    {
        //Do we need a backend? Does it really freaking matter?
        QString ourDocString = theTextEditTab->document()->toPlainText();

#if 0 //no \r\n conversion on windows, fuck it (idk why it isn't working)
        QByteArray ourDocByteArray;
        QBuffer ourDocBuffer(&ourDocByteArray);
        ourDocBuffer.open(QIODevice::ReadWrite | QIODevice::Text);
        QTextStream ourDocStringToByteArrayConverter(&ourDocBuffer); //also converts from 16-bit QString to utf-8
        ourDocStringToByteArrayConverter << ourDocString;
#endif
        QByteArray ourDocByteArray = ourDocString.toUtf8();

        QString filePathTemplate;
        if(!filePathTemplateFromCurrentDateTime(&filePathTemplate))
        {
            QMessageBox::critical(this, tr("Unable to get template for QTemporaryFile"), tr("Failed to get a good template for saving. This should never happen. Copy your file contents into a different text editor (or try saving somewhere else?).  FILE NOT SAVED"));
            return;
        }
#if 0 //racy. might exist by the time we get to open
        if(QFile::exists(absoluteAutoSaveFilePath))
        {
            QMessageBox::warning(this, tr("Auto-Filename Already Exists?"), tr("The auto-filename '") + absoluteAutoSaveFilePath + tr("' already exists. This is strange considering it's a combination of the sha1 of the content and the current datetime. This should never happen, but you should know that we did not attempt to overwrite the file, which means that if this is some strange collission that your buffer might not be persisted to disk. Good luck."));
            return;
        }
        QFile autoFilenameFile(filePathTemplate);
        if(!autoFilenameFile.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error: File Open Failed"), tr("Failed to open for writing: '") + filePathTemplate + tr("'"));
            return;
        }
#endif
        QTemporaryFile tempFileButJkWeWantToKeepIt(filePathTemplate);
        if(!tempFileButJkWeWantToKeepIt.open())
        {
            QMessageBox::critical(this, tr("QTemporaryFile failed to open"), tr("QTemporaryFile failed to open(). This should never happen. Maybe your filesystem is read-only? FILE NOT SAVED"));
            return;
        }
        if(tempFileButJkWeWantToKeepIt.write(ourDocByteArray) != ourDocByteArray.size())
        {
            QMessageBox::critical(this, tr("Error: Write Failed"), tr("Failed to write the contents of: '") + tempFileButJkWeWantToKeepIt.fileName() + tr("'.  FILE NOT SAVED"));
            return;
        }
        if(!tempFileButJkWeWantToKeepIt.flush())
        {
            QMessageBox::critical(this, tr("Error: Flush Failed"), tr("Failed to flush the contents of: '") + tempFileButJkWeWantToKeepIt.fileName() + tr("'. FILE NOT SAVED"));
            return;
        }
        //TODOmb: fsync?
        tempFileButJkWeWantToKeepIt.setAutoRemove(false); //jk lol
    }

    m_AutoFilenameSaveAction->setDisabled(true); //visual feedback of successful save, and whether or not we can save
    theTextEditTab->setTextHasBeenEditted(false);
}
