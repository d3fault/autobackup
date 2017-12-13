#ifndef AUTOGITSUBMODULEBACKUPHALPER2_RECURSIVECOMMITTER_H
#define AUTOGITSUBMODULEBACKUPHALPER2_RECURSIVECOMMITTER_H

#include <QObject>

class AutoGitSubmoduleBackupHalper2_RecursiveCommitter : public QObject
{
    Q_OBJECT
public:
    explicit AutoGitSubmoduleBackupHalper2_RecursiveCommitter(QObject *parent = 0);
private:
    bool recursivelyCommitActual(const QString &sourceDirWhichIsMyMonolithicAutobackupDir, const QString &destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules);
    static inline QString appendSlashIfNecessary(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        QString ret = inputString + "/";
        return ret;
    }
signals:
    void recursivelyCommitFinished(bool success);
public slots:
    void recursivelyCommit(const QString &sourceDirWhichIsMyMonolithicAutobackupDir, const QString &destDirWhichContainsAsuperprojectAndAboatloadOfSha1hexIdentifiedBareGitSubmodules);
};

#endif // AUTOGITSUBMODULEBACKUPHALPER2_RECURSIVECOMMITTER_H
