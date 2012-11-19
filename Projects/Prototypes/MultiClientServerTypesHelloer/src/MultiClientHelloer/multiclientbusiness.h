#ifndef MULTICLIENTBUSINESS_H
#define MULTICLIENTBUSINESS_H

#include <QObject>
#include <QIODevice>
#include <QHash>

#include "multiclienthelloer.h"

class MultiClientBusiness : public QObject
{
    Q_OBJECT
public:
    explicit MultiClientBusiness(QObject *parent = 0);
private:
    MultiClientHelloer m_ClientHelloer;

    QHash<QIODevice*,quint16> m_ActiveConnectionIdsByIODevice;
signals:
    void d(const QString &);
public slots:
    void addSslClient();
    void newConnectionPassedHelloPhase(QIODevice *theConnection, quint32 clientId);
    void serverSentUsData();
};

#endif // MULTICLIENTBUSINESS_H
