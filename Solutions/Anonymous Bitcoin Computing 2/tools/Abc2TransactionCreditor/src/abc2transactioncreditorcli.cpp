#include "abc2transactioncreditorcli.h"

#include <QCoreApplication>

#include "abc2transactioncreditor.h"

Abc2TransactionCreditorCli::Abc2TransactionCreditorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
    , m_Verbose(false)
{
    QStringList argz = QCoreApplication::arguments();
    if(argz.contains("--verbose"))
        m_Verbose = true;
    Abc2TransactionCreditor *transactionCreditor = new Abc2TransactionCreditor(m_Verbose, this);
    connect(transactionCreditor, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(transactionCreditor, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(transactionCreditor, SIGNAL(transactionCredittingFinished(bool)), this, SLOT(emitDoneAndQuit(bool)));
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
void Abc2TransactionCreditorCli::emitDoneAndQuit(bool success)
{
    if(success)
    {
        if(m_Verbose)
        {
            handleO("done");
        }
    }
    else
    {
        handleO("done but failed (neighbor may have taken over...)"); //TODOooptional: differentiate between "neighbor taking over" and "db error/etc"
    }

    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
