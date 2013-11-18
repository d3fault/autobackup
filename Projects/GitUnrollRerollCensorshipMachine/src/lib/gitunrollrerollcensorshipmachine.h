#ifndef GITUNROLLREROLLCENSORSHIPMACHINE_H
#define GITUNROLLREROLLCENSORSHIPMACHINE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCryptographicHash>
#include <QDateTime>
#include <QStringList>
#include <QList>
#include <QScopedPointer>

#include "easytreeparser.h"
#include "easytreehashitem.h"
#include "githelper.h"

class GitUnrollRerollCensorshipMachine : public QObject
{
    Q_OBJECT
public:
    explicit GitUnrollRerollCensorshipMachine(QObject *parent = 0);
private:
    GitHelper m_GitHelper;
    QString getUnusedFilename(QDir dirToGetUnusedFilenameIn);
    quint8 m_UnusedFilanameNonce;
    QString appendSlashIfNeeded(QString input);
signals:
    void d(const QString &);
public slots:
    void unrollRerollGitRepoCensoringAtEachCommit(QString filePathToListOfFilepathsToCensor, QString absoluteSourceGitDirToCensor, QString absoluteDestinationGitDirCensored, QString absoluteWorkingDir);
};

#endif // GITUNROLLREROLLCENSORSHIPMACHINE_H
