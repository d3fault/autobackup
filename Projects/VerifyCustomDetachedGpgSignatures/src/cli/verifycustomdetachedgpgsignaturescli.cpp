#include "verifycustomdetachedgpgsignaturescli.h"

#include <QCoreApplication>

#include "verifycustomdetachedgpgsignatures.h"

VerifyCustomDetachedGpgSignaturesCli::VerifyCustomDetachedGpgSignaturesCli(QObject *parent)
    : QObject(parent)
    , m_Quitting(false)
{
    VerifyCustomDetachedGpgSignatures *verifyCustomDetachedGpgSignatures = new VerifyCustomDetachedGpgSignatures(this);
    connect(verifyCustomDetachedGpgSignatures, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(verifyCustomDetachedGpgSignatures, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(verifyCustomDetachedGpgSignatures, SIGNAL(doneVerifyingCustomDetachedGpgSignatures(bool)), this, SLOT(handleDoneVerifyingCustomDetachedGpgSignatures(bool)));
    connect(this, &VerifyCustomDetachedGpgSignaturesCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection); //TODOqt4: just eh a method that does the same
}
void VerifyCustomDetachedGpgSignaturesCli::handleO(const QString &msg)
{
    m_StdOut << msg;
}
void VerifyCustomDetachedGpgSignaturesCli::handleE(const QString &msg)
{
    m_StdErr << msg;
}
void VerifyCustomDetachedGpgSignaturesCli::handleDoneVerifyingCustomDetachedGpgSignatures(bool success)
{
    if(!m_Quitting)//because this slot might be called more than once xD -- the backend might emit 'done' multiple times in error conditions
    {
        emit exitRequested(success ? 0 : 1);
        m_Quitting = true;
    }
}


