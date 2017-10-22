#ifndef STUPIDKEYVALUECONTENTTRACKERCLI_H
#define STUPIDKEYVALUECONTENTTRACKERCLI_H

#include <QObject>

#include <QTextStream>
#include <QStringList>
#include <QScopedPointer>

#include "standardinputnotifier.h"

class StupidKeyValueContentTrackerCli : public QObject
{
    Q_OBJECT
public:
    explicit StupidKeyValueContentTrackerCli(QObject *parent = nullptr);
    void main();
private:
    StandardInputNotifier *m_StandardInputNotifier;
    QTextStream m_StdErr;
    QTextStream m_StdOut;
    QStringList m_AppArgs;
    bool m_Interactive;

    void showUsage();
    void myE(const QString &msg);
    void processArgs();
    void processCommandAndCommandArgs(QStringList commandAndCommandArgs);
    void quitIfNotInteractive(bool success);
signals:
    void initializeRequested();
    void addRequested(const QString &key, const QString &data);
    void modifyRequested(const QString &key, const QString &newValue);
    void removeKeyRequested(const QString &key);
    void commitRequested(const QString & commitMessage);
    void readKeyRequested(const QString & key, const QString & revision);
    void exitRequested(int posixExitCode);
public slots:
    void handleE(QString msg);
    void handleO(QString msg);
    void handleInitializationFinished(bool success);
    void handleAddFinished(bool success);
    void handleModifyFinished(bool success);
    void handleRemoveKeyFinished(bool success);
    void handleCommitFinished(bool success);
    void handleReadKeyFinished(bool success, QString key, QString revision, QString data);
private slots:
    void handleStandardInputReceivedLine(const QString &line);
};

#endif // STUPIDKEYVALUECONTENTTRACKERCLI_H
