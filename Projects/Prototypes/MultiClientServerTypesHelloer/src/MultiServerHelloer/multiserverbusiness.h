#ifndef MULTISERVERBUSINESS_H
#define MULTISERVERBUSINESS_H

#include <QObject>
#include <QHash>

#include "multiserverhelloer.h"

//this class represents the same as "ibankserverprotocolknower" in Rpc Generator Compiling, server. I will soon be taking the "i" out of the name because it's only derivative (ssltcpserverandbankserverprotocolknower) will become a child (of sorts, minus the protocol knower portion) of the MultiServerHelloer. We need to be on the same thread as the helloer because we at some point need to atomically disconnect the helloer from the readyRead and connect readyRead to our own business slot that can handle messages. To guarantee that they're on the same thread we COULD use the "interface" model (pass ourselves as an interface to the Helloer), but with careful coding signals/slots can be used... which is better design anyways
class MultiServerBusiness : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerBusiness(QObject *parent = 0);
private:
    MultiServerHelloer m_Helloer;

    //this is only for assigning a clientId when reading the message off the network. When sending to the network, we only need it's clientId and the MultiServer figures out the QIODevice for us, ****WHICH MAY HAVE CHANGED IN THE MEANTIME****
    QHash<QIODevice*,quint16> m_ActiveConnectionIdsByIODevice;
signals:
    void d(const QString &);
public slots:
    void startAll3Listening();
    void newConnectionPassedHelloPhase(QIODevice *theConnection, quint16 clientId);
    void clientSentUsData();
};

#endif // MULTISERVERBUSINESS_H
