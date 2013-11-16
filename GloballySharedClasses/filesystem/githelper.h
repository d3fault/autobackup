#ifndef GITHELPER_H
#define GITHELPER_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QTextStream>
#include <QList>

struct GitCommitIdTimestampAndMessage
{
    QString commitId;
    QString commitDate;
    QString commitMessage;
};
class GitHelper : public QObject
{
    Q_OBJECT
public:
    explicit GitHelper(QObject *parent = 0);
    void changeWorkingDirectory(QString newWorkingDirectory);
    bool gitClone(QString srcRepoAbsolutePath, QString destRepoAbsolutePath, QString optionalWorkTree);
    bool gitInit(QString semiOptionaldirToInit, QString optionalDetachedAssociatedGitFolderLocation);
    bool gitLogReturningCommitIdsAuthorDateAndCommitMessage(QList<GitCommitIdTimestampAndMessage*> *commitIdsStringListToFill, QString semiOptionalDirToRunGitLogIn);
    bool gitCheckout(QString commitIdToCheckout, QString semiOptionalWorkTree, QString optionalDetachedAssociatedGitFolderLocation);
    bool gitAdd(QString pathToRunGitAddIn, QString optionalDetachedAssociatedGitFolderLocation);
    bool gitCommit(QString workTree, QString gitDir, QString commitMessage, QString commitDate, QString author);
private:
    QProcess m_GitProcess;
    static const QString m_GitBinaryFilePath;
signals:
    void d(const QString &);
};

#endif // GITHELPER_H
