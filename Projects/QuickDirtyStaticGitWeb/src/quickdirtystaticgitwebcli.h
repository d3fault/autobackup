#ifndef QUICKDIRTYSTATICGITWEBCLI_H
#define QUICKDIRTYSTATICGITWEBCLI_H

#include <QObject>

#include <QTextStream>

class QuickDirtyStaticGitWebCli : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyStaticGitWebCli(QObject *parent = 0);
private:
    QTextStream m_StdErr;

    void usage();
private slots:
    void handleE(const QString &msg);
    void handleFinishedGeneratingStaticGitWeb(bool success);
};

#endif // QUICKDIRTYSTATICGITWEBCLI_H
