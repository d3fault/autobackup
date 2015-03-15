#include "recursivelyverifycustomdetachedgpgsignaturescli.h"

#include <QCoreApplication>

#include "recursivelyverifycustomdetachedgpgsignatures.h"

RecursivelyVerifyCustomDetachedGpgSignaturesCli::RecursivelyVerifyCustomDetachedGpgSignaturesCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
    , m_Quitting(false)
{
    RecursivelyVerifyCustomDetachedGpgSignatures *recursivelyVerifyCustomDetachedGpgSignatures = new RecursivelyVerifyCustomDetachedGpgSignatures(this);
    connect(recursivelyVerifyCustomDetachedGpgSignatures, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(recursivelyVerifyCustomDetachedGpgSignatures, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(recursivelyVerifyCustomDetachedGpgSignatures, SIGNAL(doneRecursivelyVerifyCustomDetachedGpgSignatures(bool)), this, SLOT(handleDoneRecursivelyVerifyCustomDetachedGpgSignatures(bool)));
    connect(this, &RecursivelyVerifyCustomDetachedGpgSignaturesCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection); //TODOqt4: just eh a method that does the same

    QStringList argz = qApp->arguments();
    if(argz.length() != 3)
    {
        handleE("Usage: VerifyCustomDetachedGpgSignaturesCli dir sigsFile");
        emit exitRequested(1);
        return;
    }
    QMetaObject::invokeMethod(recursivelyVerifyCustomDetachedGpgSignatures, "recursivelyVerifyCustomDetachedGpgSignatures", Q_ARG(QString,argz.at(1)), Q_ARG(QString,argz.at(2)));
}
void RecursivelyVerifyCustomDetachedGpgSignaturesCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void RecursivelyVerifyCustomDetachedGpgSignaturesCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void RecursivelyVerifyCustomDetachedGpgSignaturesCli::handleDoneRecursivelyVerifyCustomDetachedGpgSignatures(bool success)
{
    if(!m_Quitting)//because this slot might be called more than once xD -- the backend might emit 'done' multiple times in error conditions
    {
        emit exitRequested(success ? 0 : 1);
        m_Quitting = true;
    }
}


