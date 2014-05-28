#include "d3faultpubliclicensecopyrightheaderbulkprependercli.h"

#include <QCoreApplication>

#include "d3faultpubliclicensecopyrightheaderbulkprepender.h"

D3faultPublicLicenseCopyrightHeaderBulkPrependerCli::D3faultPublicLicenseCopyrightHeaderBulkPrependerCli(QObject *parent)
    : QObject(parent)
    , m_D3faultPublicLicenseCopyrightHeaderBulkPrepender(new D3faultPublicLicenseCopyrightHeaderBulkPrepender(this))
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    connect(m_D3faultPublicLicenseCopyrightHeaderBulkPrepender, SIGNAL(bulkPrependD3faultPublicLicenseCopyrightHeaderFinished(bool)), this, SLOT(handleD3faultPublicLicenseCopyrightHeaderBulkPrependerFinished(bool)));
    connect(m_D3faultPublicLicenseCopyrightHeaderBulkPrepender, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(m_D3faultPublicLicenseCopyrightHeaderBulkPrepender, SIGNAL(e(QString)), this, SLOT(handleE(QString)));

    QStringList argz = QCoreApplication::arguments();
    if(argz.size() < 2)
    {
        usage();
        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
        return;
    }
    QString dirToPrependTo = argz.at(1);

    QString optionalAlternateCopyrightHeaderFilePath;
    if(argz.size() > 2)
    {
        optionalAlternateCopyrightHeaderFilePath = argz.at(2);
    }

    QMetaObject::invokeMethod(m_D3faultPublicLicenseCopyrightHeaderBulkPrepender, "bulkPrependD3faultPublicLicenseCopyrightHeader", Q_ARG(QString, dirToPrependTo), Q_ARG(QString, optionalAlternateCopyrightHeaderFilePath));
}
void D3faultPublicLicenseCopyrightHeaderBulkPrependerCli::usage()
{
    QString usageString = "Usage: D3faultPublicLicenseCopyrightHeaderBulkPrependerCli dirToRecursivelyPrependTo [optionalAlternateCopyrightHeaderFilePath]\n";
    handleO(usageString);
}
void D3faultPublicLicenseCopyrightHeaderBulkPrependerCli::handleD3faultPublicLicenseCopyrightHeaderBulkPrependerFinished(bool success)
{
    if(success)
    {
        handleO("D3faultPublicLicenseCopyrightHeaderBulkPrependerCli finished successfully");
    }
    else
    {
        handleE("D3faultPublicLicenseCopyrightHeaderBulkPrependerCli failed");
    }
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void D3faultPublicLicenseCopyrightHeaderBulkPrependerCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void D3faultPublicLicenseCopyrightHeaderBulkPrependerCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
