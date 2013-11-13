#ifndef GITUNROLLREROLLCENSORSHIPMACHINECLI_H
#define GITUNROLLREROLLCENSORSHIPMACHINECLI_H

#include <QObject>
#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QDir>

#include "gitunrollrerollcensorshipmachine.h"

class GitUnrollRerollCensorshipMachineCli : public QObject
{
    Q_OBJECT
public:
    explicit GitUnrollRerollCensorshipMachineCli(QObject *parent = 0);
private:
    GitUnrollRerollCensorshipMachine *m_Business;
    QTextStream m_StdOutStream;
    void usage();
    void quit();
signals:
    
public slots:
    void parseArgsAndThenDoGitUnrollRerollCensoring();
    void handleD(const QString &msg);
    void handleAboutToQuit();
};

#endif // GITUNROLLREROLLCENSORSHIPMACHINECLI_H
