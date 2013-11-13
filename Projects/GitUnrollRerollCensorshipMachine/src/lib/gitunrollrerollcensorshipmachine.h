#ifndef GITUNROLLREROLLCENSORSHIPMACHINE_H
#define GITUNROLLREROLLCENSORSHIPMACHINE_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#include "easytreeparser.h"
#include "githelper.h"

class GitUnrollRerollCensorshipMachine : public QObject
{
    Q_OBJECT
public:
    explicit GitUnrollRerollCensorshipMachine(QObject *parent = 0);
private:
    GitHelper m_GitHelper;
signals:
    void d(const QString &);
public slots:
    void unrollRerollGitRepoCensoringAtEachCommit(QString filePathToListOfFilepathsToCensor, QString sourceGitDirToCensor, QString destinationGitDirCensored, QString workingDir);
};

#endif // GITUNROLLREROLLCENSORSHIPMACHINE_H
