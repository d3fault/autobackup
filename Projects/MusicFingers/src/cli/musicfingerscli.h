#ifndef MUSICFINGERSCLI_H
#define MUSICFINGERSCLI_H

#include <QObject>
#include <QTextStream>

class MusicFingersCli : public QObject
{
    Q_OBJECT
public:
    explicit MusicFingersCli(QObject *parent = 0);
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void cliUsage();
signals:
    void queryMusicFingerPositionRequested(int fingerIndex);
private slots:
    void handleStandardInputLineReceived(const QString &line);
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void quit();
};

#endif // MUSICFINGERSCLI_H
