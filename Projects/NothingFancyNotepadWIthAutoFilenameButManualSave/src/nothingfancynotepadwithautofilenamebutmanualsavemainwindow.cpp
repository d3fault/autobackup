#include "nothingfancynotepadwithautofilenamebutmanualsavemainwindow.h"

#include <QPlainTextEdit>
#include <QAction>
#include <QToolBar>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCryptographicHash> //NVM(since i'm not storing milliseconds, i don't want to RISK 16 bits not being enough):git stores this, so we'll only use a crc32 instead
#include <QDateTime>
#include <QStyle>
//#include <QBuffer>


//TODOreq: color indication of whether or not a file (or perhaps 'tab') is saved
//TODOreq: auto save and manual save. the former when doing mind dump, the latter used when writing project designs and wanting good filenames
//TODOreq: folder-ize?
NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_MainTextEdit(new QPlainTextEdit())
{
    setWindowTitle(tr("Nothing Fancy Notepad with Auto-Filename Save"));
    setCentralWidget(m_MainTextEdit);

    createActions();
    createToolbars();
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::createActions()
{
    m_AutoFilenameSaveAction = new QAction(tr("Auto-Filename Save"), this);
    m_AutoFilenameSaveAction->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    connect(m_AutoFilenameSaveAction, SIGNAL(triggered()), this, SLOT(handleAutoFilenameSaveActionTriggered()));
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::createToolbars()
{
    QToolBar *documentOperationsToolbar = addToolBar(tr("Document Operations Toolbar"));
    documentOperationsToolbar->addAction(m_AutoFilenameSaveAction);
}
QString NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::autoFileNameFromContentsAndCurrentTime(const QByteArray &fileContents)
{
    QString ret("AutoFilenameSaveAt-" + QString::number(QDateTime::currentMSecsSinceEpoch() / static_cast<qint64>(1000)) + "-");
    //qint16 contentsCrc16 = qChecksum(fileContents.constData(), fileContents.length());
    QByteArray contentHash = QCryptographicHash::hash(fileContents, QCryptographicHash::Md5);
    QByteArray contentHashHex = contentHash.toHex();
    ret.append(contentHashHex);
    ret.append(".txt");
    return ret;
}
void NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow::handleAutoFilenameSaveActionTriggered()
{
    //Do we need a backend? Does it really freaking matter?
    QString ourDocString = m_MainTextEdit->document()->toPlainText();

#if 0 //no \r\n conversion on windows, fuck it (idk why it isn't working)
    QByteArray ourDocByteArray;
    QBuffer ourDocBuffer(&ourDocByteArray);
    ourDocBuffer.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream ourDocStringToByteArrayConverter(&ourDocBuffer); //also converts from 16-bit QString to utf-8
    ourDocStringToByteArrayConverter << ourDocString;
#endif
    QByteArray ourDocByteArray = ourDocString.toUtf8();

    QString absoluteAutoSaveFilePath = /*TODOreq: appendSlashIfNeeded(currentSettingsProfile.autoSaveDir())*/ "/run/shm/" + autoFileNameFromContentsAndCurrentTime(ourDocByteArray);
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
    if(!autoFilenameFile.write(ourDocByteArray) != ourDocByteArray.size())
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
