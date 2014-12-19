#include "abc2transactioncreditorcli.h"

#include <QCoreApplication>

#include "abc2transactioncreditor.h"

Abc2TransactionCreditorCli::Abc2TransactionCreditorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
{
    Abc2TransactionCreditor *transactionCreditor = new Abc2TransactionCreditor(this);
    connect(transactionCreditor, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(transactionCreditor, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(transactionCreditor, SIGNAL(transactionCredittingFinished(bool)), qApp, SLOT(quit()), Qt::QueuedConnection);
    QMetaObject::invokeMethod(transactionCreditor, "creditTransactions");
}
void Abc2TransactionCreditorCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void Abc2TransactionCreditorCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
