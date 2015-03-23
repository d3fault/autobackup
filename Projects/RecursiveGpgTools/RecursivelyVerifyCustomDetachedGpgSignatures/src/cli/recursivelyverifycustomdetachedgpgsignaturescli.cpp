#include "recursivelyverifycustomdetachedgpgsignaturescli.h"

#include <QCoreApplication>

#include "recursivelyverifycustomdetachedgpgsignatures.h"

#define ERRRRRR { cliUsage(); emit exitRequested(1); return; }

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
    argz.removeFirst(); //app filename

    QStringList excludeEntries;
    int excludeIndex;
    while((excludeIndex = argz.indexOf(RecursiveGpgTools_EXCLUDE_ARG)) > -1)
    {
        argz.removeAt(excludeIndex);
        if(argz.size() < (excludeIndex+1))
            ERRRRRR
        excludeEntries << argz.takeAt(excludeIndex);
    }

    if(argz.length() != 2)
        ERRRRRR
    QMetaObject::invokeMethod(recursivelyVerifyCustomDetachedGpgSignatures, "recursivelyVerifyCustomDetachedGpgSignatures", Q_ARG(QString,argz.at(0)), Q_ARG(QString,argz.at(1)), Q_ARG(QStringList, excludeEntries));
}
void RecursivelyVerifyCustomDetachedGpgSignaturesCli::cliUsage()
{
    handleE("Usage: VerifyCustomDetachedGpgSignaturesCli dir sigsFile");
    handleE("");
    handleE(RecursiveGpgTools_EXCLUDE_ARG "\tExclude files. Specified as a path relative to the dir to recursively verify (without a leading './'). You can use this flag multiple times. Note, it only affects files on the filesystem. ie, suppresses the warning 'file x was on filesystem but not in sigs file'. It does not exclude any entry in the sigsfile"); //TODOoptional: exclude entries in sigs file
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


