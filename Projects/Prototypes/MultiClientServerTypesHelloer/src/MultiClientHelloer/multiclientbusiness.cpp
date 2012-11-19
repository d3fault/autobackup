#include "multiclientbusiness.h"

MultiClientBusiness::MultiClientBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_ClientHelloer, SIGNAL(connectionHasBeenHelloedAndIsReadyForAction(QIODevice*,quint32)), this, SLOT(newConnectionPassedHelloPhase(QIODevice*,quint32)));
    connect(&m_ClientHelloer, SIGNAL(d(const QString &)), this, SIGNAL(d(const QString &)));
}
void MultiClientBusiness::addSslClient()
{
    m_ClientHelloer.addSslClient();
}
void MultiClientBusiness::newConnectionPassedHelloPhase(QIODevice *theConnection, quint32 clientId)
{
}
void MultiClientBusiness::serverSentUsData()
{
}
