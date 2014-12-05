#include "abc2withdrawrequestprocessorcli.h"

#include <QCoreApplication>

#include "abc2withdrawrequestprocessor.h"

Abc2WithdrawRequestProcessorCli::Abc2WithdrawRequestProcessorCli(QObject *parent)
    : QObject(parent)
    , m_StdErr(stderr)
    , m_StdOut(stdout)
{
    Abc2WithdrawRequestProcessor *processor = new Abc2WithdrawRequestProcessor(this);

    connect(processor, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(processor, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(processor, SIGNAL(withdrawalRequestProcessingFinished(bool)), qApp, SLOT(quit()), Qt::QueuedConnection);

    QMetaObject::invokeMethod(processor, "processWithdrawalRequests", Qt::QueuedConnection);
}
void Abc2WithdrawRequestProcessorCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void Abc2WithdrawRequestProcessorCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
