#include "sftpuploadandrenameallvideosegmentsinfoldercli.h"

#include <QCoreApplication>

#include "sftpuploadandrenameallvideosegmentsinfolder.h"

#define ERRRRRRR_SftpUploadAndRenameAllVideoSegmentsInFolderCli { cliUsage(); QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection); return; } //this is one line when invoked. macros ftw; your inline functions can't do THAT, hah! :-P. aww shit mfw just realized i'm gonna have namespace collisions with these eventually :( (SOLVED. i put the shit after the errrr part to make auto-completion easier to remember/trigger (i always did say, there's nothin' like a good macro. well i never said it before, but i'm sayin' it nao))

SftpUploadAndRenameAllVideoSegmentsInFolderCli::SftpUploadAndRenameAllVideoSegmentsInFolderCli(QObject *parent)
    : QObject(parent)
{
    SftpUploadAndRenameAllVideoSegmentsInFolder *sftpUploadAndRenameAllVideoSegmentsInFolder = new SftpUploadAndRenameAllVideoSegmentsInFolder(this);
    connect(sftpUploadAndRenameAllVideoSegmentsInFolder, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(sftpUploadAndRenameAllVideoSegmentsInFolder, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(sftpUploadAndRenameAllVideoSegmentsInFolder, SIGNAL(quitRequested()), qApp, SLOT(quit()), Qt::QueuedConnection);

    QStringList arguments = qApp->arguments();
    arguments.removeFirst(); //app filename

    if(arguments.size() != 4)
        ERRRRRRR_SftpUploadAndRenameAllVideoSegmentsInFolderCli

    //TODOoptional: sftp bin as cli arg

    QMetaObject::invokeMethod(sftpUploadAndRenameAllVideoSegmentsInFolder, "startSftpUploadAndRenameAllVideoSegmentsInFolder", Q_ARG(QString, arguments.at(0) /*dirContainingTheFiles*/), Q_ARG(QString, arguments.at(1) /*remoteDestinationToUploadTo*/), Q_ARG(QString, arguments.at(2) /*remoteDestinationToMoveTo*/), Q_ARG(QString, arguments.at(3) /*userHostPathComboSftpArg*/));
}
void SftpUploadAndRenameAllVideoSegmentsInFolderCli::cliUsage()
{
    handleE("Usage: SftpUploadAndRenameAllVideoSegmentsInFolderCli dirContainingTheFiles remoteDestinationToUploadTo remoteDestinationToMoveTo userHostPathComboSftpArg");
}
void SftpUploadAndRenameAllVideoSegmentsInFolderCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void SftpUploadAndRenameAllVideoSegmentsInFolderCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
