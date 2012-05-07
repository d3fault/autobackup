#ifndef BANKSERVER_H
#define BANKSERVER_H

#include <QObject>
#include <QThread>
#include <QDebug>

#include "bank.h"
#include "clientshelper.h"

class BankServer : public QObject
{
    Q_OBJECT
public:
    explicit BankServer(QObject *parent = 0);
    void startListening();
private:
    QThread *m_BankThread;
    Bank *m_Bank;
    QThread *m_ClientsThread;
    ClientsHelper *m_Clients;
signals:

private slots:
    void handleD(const QString &msg);
};

#endif // BANKSERVER_H
