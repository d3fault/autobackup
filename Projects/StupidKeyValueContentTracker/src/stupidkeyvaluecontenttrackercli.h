#ifndef STUPIDKEYVALUECONTENTTRACKERCLI_H
#define STUPIDKEYVALUECONTENTTRACKERCLI_H

#include <QObject>
#include <QTextStream>

class StupidKeyValueContentTrackerCli : public QObject
{
    Q_OBJECT
public:
    explicit StupidKeyValueContentTrackerCli(QObject *parent = nullptr);
    void main();
private:
    QTextStream m_StdErr;
    QTextStream m_StdOut;
signals:
    void initializeRequested();
    void addRequested(const QString & key, const QString & data);
    void commitRequested(const QString & commitMessage);
    void readKeyRequested(const QString & key, const QString & revision);
    void exitRequested(int posixExitCode);
public slots:
    void handleE(QString msg);
    void handleO(QString msg);
    void handleInitializationFinished(bool success);
    void handleAddFinished(bool success);
    void handleCommitFinished(bool success);
    void handleReadKeyFinished(bool success, QString key, QString revision, QString data);
};

#endif // STUPIDKEYVALUECONTENTTRACKERCLI_H
