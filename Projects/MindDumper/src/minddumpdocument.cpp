#include "minddumpdocument.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QScopedPointer>
#include <QDir>
#include <QTemporaryFile>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QKeySequence>

MindDumpDocument::MindDumpDocument(const QString &tabTitle, const QString &mindDumpDirectoryWithSlashAppended, QWidget *parent)
    : QWidget(parent)
    , m_TabTitle(tabTitle)
    , m_MindDumpDirectoryWithSlashAppended(mindDumpDirectoryWithSlashAppended)
    , m_IsSaved(false)
    , m_TimestampOfFirstKeystroke(0)
{
    QVBoxLayout *myLayout = new QVBoxLayout(this);
    myLayout->addWidget(m_FilenameLineEdit = new QLineEdit()); //no naked new!
    m_FilenameLineEdit->setPlaceholderText(tr("Optional Filename..."));
    m_FilenameLineEdit->setToolTip(tr("A-Z and 0-9; Spaces, underscores and hyphens will be converted into dots. Everything else is discarded"));
    myLayout->addWidget(m_DreamCheckbox = new QCheckBox(tr("&Dream:")));
    m_DreamCheckbox->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(m_DreamCheckbox, SIGNAL(toggled(bool)), this, SLOT(setFocusOnDocument())); //the shortcut gives the checkbox focus, we only want to toggle it
    myLayout->addWidget(m_Document = new QPlainTextEdit(), 1);

    connect(m_Document, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));
}
void MindDumpDocument::setFocusOnDocument()
{
    m_Document->setFocus();
}
QString MindDumpDocument::tabTitle() const
{
    return m_TabTitle;
}
bool MindDumpDocument::isSaved() const
{
    return m_IsSaved;
}
bool MindDumpDocument::charOfFilenameisSane(const QString &charOfFilename)
{
    for(int i = Qt::Key_A; i <= Qt::Key_Z; ++i)
    {
        if(QKeySequence(i).toString().toLower() == charOfFilename.toLower())
            return true;
    }
    for(int i = Qt::Key_0; i <= Qt::Key_9; ++i)
    {
        if(QKeySequence(i).toString() == charOfFilename)
            return true;
    }
    if(charOfFilename == "_" || charOfFilename == "-" || charOfFilename == " " || charOfFilename == ".")
        return true;
    return false;
}
QString MindDumpDocument::filterFilenameChar(const QString &charOfFilenameToFilter)
{
    if(charOfFilenameToFilter == "_" || charOfFilenameToFilter == "-" || charOfFilenameToFilter == " ")
        return ".";
    return charOfFilenameToFilter;
}
QString MindDumpDocument::fileNameSanitized(const QString &inputFileName)
{
    QString ret;
    for(int i = 0; i < inputFileName.length(); ++i)
    {
        const QChar &currentLetter = inputFileName.at(i);
        if(charOfFilenameisSane(currentLetter))
            ret.append(filterFilenameChar(currentLetter));
    }
    return ret;
}
void MindDumpDocument::saveAndFudgeLastModifiedTimestamp()
{
    QScopedPointer<ResultEmitter> saveResultEmitter(new ResultEmitter());
    connect(saveResultEmitter.data(), SIGNAL(haveResult(bool)), this, SIGNAL(savedAndFudgedLastModifiedTimestamp(bool)));

    QString text = m_Document->document()->toPlainText();
    if(!text.trimmed().isEmpty())
    {
        QString theChosenFilePathAndName; //not known yet, QTemporaryFile choses for us
        //if((true) && (!false) && 1 == 1)
        {
            QString maybeHyphenAndFilenamePortion;
            QString sanitizedFilename = fileNameSanitized(m_FilenameLineEdit->text().trimmed());
            if(!sanitizedFilename.isEmpty())
                maybeHyphenAndFilenamePortion = "-" + sanitizedFilename;  //TODOmb: sanitization? for now I'll use dots and not spaces, but maybe in the future I should convert spaces to dots etc? TODOreq: do not allow six upper case X's in filename, since it would fuck with the temporary file's template
            QDateTime dateTimeOfFirstKeystroke = QDateTime::fromMSecsSinceEpoch(m_TimestampOfFirstKeystroke * 1000);
            QString jitCreatedYearAndMonthFoldersWithSlashAppended = m_MindDumpDirectoryWithSlashAppended + (m_DreamCheckbox->isChecked() ? "dreams/" : "") + QString::number(dateTimeOfFirstKeystroke.date().year()) + "/" + QString::number(dateTimeOfFirstKeystroke.date().month()) + "/";
            QDir yearMonthDir(jitCreatedYearAndMonthFoldersWithSlashAppended);
            if(!yearMonthDir.mkpath(jitCreatedYearAndMonthFoldersWithSlashAppended))
            {
                QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to mkpath: ") + jitCreatedYearAndMonthFoldersWithSlashAppended);
                return;
            }
            QString temporaryFileTemplatePath = jitCreatedYearAndMonthFoldersWithSlashAppended + QString::number(m_TimestampOfFirstKeystroke) + maybeHyphenAndFilenamePortion + "-XXXXXX.txt";
            QTemporaryFile file(temporaryFileTemplatePath);

            if(!file.open(/*wtf QIODevice::WriteOnly | QIODevice::Text*/))
            {
                QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to open QTemporaryFile for writing with template: ") + temporaryFileTemplatePath);
                return;
            }

            QTextStream fileStream(&file);
            fileStream << text;
            fileStream.flush();

            if(fileStream.status() != QTextStream::Ok)
            {
                QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to flush the QTextStream around file: ") + file.fileName());
                return;
            }

            if(!file.flush())
            {
                QMessageBox::critical(this, tr("Critical Error!"), tr("Failed to flush the file: ") + file.fileName());
                return;
            }
            theChosenFilePathAndName = file.fileName();
            file.setAutoRemove(false);
            file.close();
        }

        //file created and closed, now fudge timestamp to m_TimestampOfLastKeystroke
        QProcess touchProcess;
        QStringList touchProcessArgs = QStringList() << "--no-create" << "-m" << QString("--date=@" + QString::number(m_TimestampOfLastKeystroke)) << theChosenFilePathAndName;
        touchProcess.start("touch", touchProcessArgs, QIODevice::ReadOnly);
        if(!touchProcess.waitForStarted(-1))
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("The `touch` process failed to start with the arguments: ") + touchProcessArgs.join(" ") + "\n\n\n" + tr("NOTE: The file has already been written to disk and will not be removed!!!"));
            return;
        }
        if(!touchProcess.waitForFinished(-1))
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("The `touch` process failed to start with the arguments: ") + touchProcessArgs.join(" ") + "\n\n\n" + tr("NOTE: The file has already been written to disk and will not be removed!!!"));
            return;
        }
        if(touchProcess.exitStatus() != QProcess::NormalExit || touchProcess.exitCode() != 0)
        {
            QMessageBox::critical(this, tr("Critical Error!"), tr("The `touch` process exitted abnormally (exit-code=") + QString::number(touchProcess.exitCode()) + tr(") with the arguments: ") + touchProcessArgs.join(" ") + "\n\n\n" + tr("NOTE: The file has already been written to disk and will not be removed!!!"));
            return;
        }
    }
    else if(!m_FilenameLineEdit->text().trimmed().isEmpty())
    {
        //corner case: the document was empty, but the filename was not. don't save, tell them about it and they ahve to then delete the text before trying to save again
        QMessageBox::warning(this, tr("Warning!"), tr("In tab ") + m_TabTitle + tr(", you have a filename but no file contents. Delete the filename and try again")); //TODOoptional: ask them here and now how to proceed? Discard yes/no whatever
        return;
    }

    saveResultEmitter->setSuccess(true);
    m_IsSaved = true;
}
void MindDumpDocument::handleTextChanged()
{
    //m_TimestampOfFirstKeystroke
    bool documentIsEmpty = m_Document->document()->isEmpty();
    if(m_TimestampOfFirstKeystroke == 0 && (!documentIsEmpty))
        m_TimestampOfFirstKeystroke = QDateTime::currentMSecsSinceEpoch() / 1000;
    else if(documentIsEmpty)
        m_TimestampOfFirstKeystroke = 0;


    m_TimestampOfLastKeystroke = QDateTime::currentMSecsSinceEpoch() / 1000;
}
