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
//#include <QBuffer>

#include "newprofiledialog.h"
#include "nothingfancyplaintextedit.h"

//TODOoptional: could be specifiable in the user profile
#define NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Prefix "AutoFilenameSaveAt-"
#define NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Extension ".txt"

//TODOreq: color indication of whether or not a file (or perhaps 'tab') is saved
//TODOreq: auto save and manual save. the former when doing mind dump, the latter used when writing project designs and wanting good filenames
//TODOoptional: when a close attempt is detected, iterate all open docs and see if they need to be saved. show a dialog with each unsaved doc. each doc has a radio selection for "auto save" and "manual save". next to manual save and only enabled when relevant is a line edit (or browse button or both?)
NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow(QWidget *parent)
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
        m_CurrentProfileBaseDir = newProfileDialog.newProfileBaseDir();
        m_FolderizeBaseDir = newProfileDialog.folderizeBaseDir();
        settings.setValue("lastUsedProfile", m_CurrentProfile);

        settings.beginGroup(m_CurrentProfile);
        settings.setValue("baseDir", m_CurrentProfileBaseDir);
        settings.setValue("folderizeBaseDir", m_FolderizeBaseDir);
        settings.endGroup();
    }
    else
    {
        //load profile
        settings.beginGroup(m_CurrentProfile);
        m_CurrentProfileBaseDir = NewProfileDialog::appendSlashIfNeeded(settings.value("baseDir").toString());
        m_FolderizeBaseDir = settings.value("folderizeBaseDir").toBool();
        settings.endGroup();
    }

    addNewFileTabAndMakeItCurrent();

    connect(m_MainTabWidget, SIGNAL(currentChanged(int)), this, SLOT(handleMainTabWidgetCurrentTabChanged(int)));
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::closeEvent(QCloseEvent *closeEvent)
{
    QSettings settings;
    settings.setValue("windowGeometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(closeEvent);
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::createActions()
{
    m_NewFileTabAction = new QAction(tr("New File"), this);
    m_NewFileTabAction->setIcon(style()->standardIcon(QStyle::SP_FileIcon));

    m_AutoFilenameSaveAction = new QAction(tr("Auto-Filename Save"), this);
    m_AutoFilenameSaveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    m_AutoFilenameSaveAction->setDisabled(true); //initially nothing to save

    connect(m_NewFileTabAction, SIGNAL(triggered()), this, SLOT(handleNewFileTabActionTriggered()));
    connect(m_AutoFilenameSaveAction, SIGNAL(triggered()), this, SLOT(handleAutoFilenameSaveActionTriggered()));
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::createToolbars()
{
    QToolBar *documentOperationsToolbar = addToolBar(tr("Document Operations Toolbar"));
    documentOperationsToolbar->setObjectName("documentOperationsToolbar");

    documentOperationsToolbar->addAction(m_NewFileTabAction);
    documentOperationsToolbar->addAction(m_AutoFilenameSaveAction);
}
QString NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::autoFileNameFromContentsAndCurrentTime(const QByteArray &fileContents)
{
    if(m_FolderizeBaseDir)
    {
        QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
        QString folderizedPath = m_CurrentProfileBaseDir
                                + QString::number(currentDateTime.date().year()) //2014
                                + QDir::separator()
                                + currentDateTime.date().toString("MM-MMMM") //01-January
                                + QDir::separator()
                                + currentDateTime.date().toString("dd")
                                + QDir::separator();

        QFileInfo folderizedPathInfo(folderizedPath);
        bool exists = folderizedPathInfo.exists();
        if(exists && !folderizedPathInfo.isDir())
        {
            QMessageBox::critical(this, tr("Error"), tr("Expected a folder, but it isn't a folder: '") + folderizedPath + tr("'"));
            return QString();
        }
        if(!exists)
        {
            QDir folderizedDir(folderizedPath);
            if(!folderizedDir.mkpath(folderizedPath))
            {
                QMessageBox::critical(this, tr("Error Creating Directories"), tr("Failed to recursively mkdir '") + folderizedPath + tr("'"));
                return QString();
            }
        }
        QByteArray contentHashHex = myHexHash(fileContents);
        QString ret(folderizedPath + NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Prefix + currentDateTime.time().toString("HH_mm_ss") + "-");
        ret.append(contentHashHex);
        ret.append(NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Extension);
        return ret;
    }
    else
    {
        QString ret(m_CurrentProfileBaseDir + NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Prefix + QString::number(QDateTime::currentMSecsSinceEpoch() / static_cast<qint64>(1000)) + "-");
        //qint16 contentsCrc16 = qChecksum(fileContents.constData(), fileContents.length());
        QByteArray contentHashHex = myHexHash(fileContents);
        ret.append(contentHashHex);
        ret.append(NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow_AutoFilename_Extension);
        return ret;
    }
}
QByteArray NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::myHexHash(const QByteArray &inputByteArray)
{
    QByteArray contentHash = QCryptographicHash::hash(inputByteArray, QCryptographicHash::Md5);
    QByteArray contentHashHex = contentHash.toHex();
    return contentHashHex;
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::addNewFileTabAndMakeItCurrent()
{
    NothingFancyPlainTextEdit *newTextEdit = new NothingFancyPlainTextEdit();
    int newTabIndex = m_MainTabWidget->addTab(newTextEdit, QString::number(m_CurrentVisibleTabIndex));
    m_MainTabWidget->setCurrentIndex(newTabIndex);
    newTextEdit->setFocus();
    ++m_CurrentVisibleTabIndex;

    connect(newTextEdit, SIGNAL(textChanged()), this, SLOT(handleMainTextEditChanged()));
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::handleMainTabWidgetCurrentTabChanged(int newIndex)
{
    NothingFancyPlainTextEdit *theTextEditTab = static_cast<NothingFancyPlainTextEdit*>(m_MainTabWidget->widget(newIndex));
    m_AutoFilenameSaveAction->setEnabled(theTextEditTab->textHasBeenEditted());
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::handleMainTextEditChanged()
{
    if(!m_AutoFilenameSaveAction->isEnabled())
    {
        m_AutoFilenameSaveAction->setEnabled(true);
    }
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::handleNewFileTabActionTriggered()
{
    addNewFileTabAndMakeItCurrent();
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::handleAutoFilenameSaveActionTriggered()
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

        QString absoluteAutoSaveFilePath = autoFileNameFromContentsAndCurrentTime(ourDocByteArray);
        if(absoluteAutoSaveFilePath.isEmpty())
            return;
        if(QFile::exists(absoluteAutoSaveFilePath))
        {
            QMessageBox::warning(this, tr("Auto-Filename Already Exists?"), tr("The auto-filename '") + absoluteAutoSaveFilePath + tr("' already exists. This is strange considering it's a combination of the sha1 of the content and the current datetime. This should never happen, but you should know that we did not attempt to overwrite the file, which means that if this is some strange collission that your buffer might not be persisted to disk. Good luck."));
            return;
        }
        QFile autoFilenameFile(absoluteAutoSaveFilePath);
        if(!autoFilenameFile.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error: File Open Failed"), tr("Failed to open for writing: '") + absoluteAutoSaveFilePath + tr("'"));
            return;
        }
        if(autoFilenameFile.write(ourDocByteArray) != ourDocByteArray.size())
        {
            QMessageBox::critical(this, tr("Error: Write Failed"), tr("Failed to write the contents of: '") + absoluteAutoSaveFilePath + tr("'"));
            return;
        }
        if(!autoFilenameFile.flush())
        {
            QMessageBox::critical(this, tr("Error: Flush Failed"), tr("Failed to flush the contents of: '") + absoluteAutoSaveFilePath + tr("'"));
            return;
        }
    }

    m_AutoFilenameSaveAction->setDisabled(true); //visual feedback of successful save, and whether or not we can save
    theTextEditTab->setTextHasBeenEditted(false);
}
