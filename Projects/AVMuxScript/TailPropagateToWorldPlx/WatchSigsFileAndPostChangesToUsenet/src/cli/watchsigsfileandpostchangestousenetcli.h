#ifndef WATCHSIGSFILEANDPOSTCHANGESTOUSENETCLI_H
#define WATCHSIGSFILEANDPOSTCHANGESTOUSENETCLI_H

#include <QObject>

#include <QTextStream>

class WatchSigsFileAndPostChangesToUsenetCli : public QObject
{
    Q_OBJECT
public:
    explicit WatchSigsFileAndPostChangesToUsenetCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void cliUsage();
signals:
    void printMessageIDsForRelativeFilePathRequested(const QString &relativeFilePath);
    void printMessageIdCurrentlyBeingPostedRequested();
    void quitCleanlyRequested();
    void exitRequested(int exitCode);
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleStandardInputReceivedLine(const QString &userInputLine);
    void handleDoneWatchingSigsFileAndPostingChangesToUsenet(bool success);
};

#endif // WATCHSIGSFILEANDPOSTCHANGESTOUSENETCLI_H
