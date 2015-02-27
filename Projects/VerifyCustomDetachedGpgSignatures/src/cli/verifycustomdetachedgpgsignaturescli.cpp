#include "verifycustomdetachedgpgsignaturescli.h"

#include <QCoreApplication>

#include "verifycustomdetachedgpgsignatures.h"

VerifyCustomDetachedGpgSignaturesCli::VerifyCustomDetachedGpgSignaturesCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
    , m_StdErr(stderr)
    , m_Quitting(false)
{
    VerifyCustomDetachedGpgSignatures *verifyCustomDetachedGpgSignatures = new VerifyCustomDetachedGpgSignatures(this);
    connect(verifyCustomDetachedGpgSignatures, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(verifyCustomDetachedGpgSignatures, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(verifyCustomDetachedGpgSignatures, SIGNAL(doneVerifyingCustomDetachedGpgSignatures(bool)), this, SLOT(handleDoneVerifyingCustomDetachedGpgSignatures(bool)));
    connect(this, &VerifyCustomDetachedGpgSignaturesCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection); //TODOqt4: just eh a method that does the same

    QStringList argz = qApp->arguments();
    if(argz.length() != 3)
    {
        handleE("Usage: VerifyCustomDetachedGpgSignaturesCli dir sigsFile");
        emit exitRequested(1);
        return;
    }
    QMetaObject::invokeMethod(verifyCustomDetachedGpgSignatures, "verifyCustomDetachedGpgSignatures", Q_ARG(QString,argz.at(1)), Q_ARG(QString,argz.at(2)));
}
void VerifyCustomDetachedGpgSignaturesCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void VerifyCustomDetachedGpgSignaturesCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void VerifyCustomDetachedGpgSignaturesCli::handleDoneVerifyingCustomDetachedGpgSignatures(bool success)
{
    if(!m_Quitting)//because this slot might be called more than once xD -- the backend might emit 'done' multiple times in error conditions
    {
        emit exitRequested(success ? 0 : 1);
        m_Quitting = true;
    }
}


