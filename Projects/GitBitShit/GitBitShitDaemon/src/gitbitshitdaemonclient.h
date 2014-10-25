#ifndef GITBITSHITDAEMONCLIENT_H
#define GITBITSHITDAEMONCLIENT_H

#include <QObject>

class QIODevice;

class GitBitShitDaemonClient : public QObject
{
    Q_OBJECT
public:
    explicit GitBitShitDaemonClient(QIODevice *client, QObject *parent = 0);
private:
    QIODevice *m_Client;
signals:
    void messageReceived(const QString &theMessage);
private slots:
    void handleReadyRead();
};

#endif // GITBITSHITDAEMONCLIENT_H
