#include "recursivelygpgsignintocustomdetachedsignaturesformatcli.h"

#include <QCoreApplication>

#include "recursivelygpgsignintocustomdetachedsignaturesformat.h"

RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
    , m_Quitting(false)
{
    RecursivelyGpgSignIntoCustomDetachedSignaturesFormat *recursivelyGpgSignIntoCustomDetachedSignaturesFormat = new RecursivelyGpgSignIntoCustomDetachedSignaturesFormat(this);
    connect(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, SIGNAL(doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool)), this, SLOT(handleDoneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool)));
    connect(this, &RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection); //TODOqt4: just eh a method that does the same

    QStringList argz = qApp->arguments();
    if(argz.length() != 3)
    {
        handleE("Usage: RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli dir sigsFile");
        emit exitRequested(1);
        return;
    }
    QMetaObject::invokeMethod(recursivelyGpgSignIntoCustomDetachedSignaturesFormat, "recursivelyGpgSignIntoCustomDetachedSignaturesFormat", Q_ARG(QString,argz.at(1)), Q_ARG(QString,argz.at(2)));
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void RecursivelyGpgSignIntoCustomDetachedSignaturesFormatCli::handleDoneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool success)
{
    if(!m_Quitting)//because this slot might be called more than once xD -- the backend might emit 'done' multiple times in error conditions
    {
        emit exitRequested(success ? 0 : 1);
        m_Quitting = true;
    }
}
