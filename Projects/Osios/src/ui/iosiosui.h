#ifndef IOSIOSUI_H
#define IOSIOSUI_H

#include <QObject>

class QLocalSocket;

class IOsiosUi: public QObject
{
    Q_OBJECT
public:
    explicit IOsiosUi(QObject *parent = 0);
    virtual ~IOsiosUi();
private:
    QLocalSocket *m_SerializedActionSender;
protected:
    QObject *m_ActualUi;
    virtual QObject *presentUi()=0;
signals:
    void thisUiIsDoneWithOsiosDaemon();
public slots:
    void handleOisiosLocalServerReadyForIOsiosUiLocalSocketConnection(const QString &localServerName);
    virtual void handleE(const QString &msg)=0;
private slots:
    void handleLocalSocketConnected();
    void handleThisUiIsDoneWithOsiosDaemon();
};

#endif // IOSIOSUI_H
