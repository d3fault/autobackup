#ifndef RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURES_H
#define RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURES_H

#include <QObject>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QHash>

class QIODevice;

#include "recursivecustomdetachedgpgsignatures.h"

class RecursivelyVerifyCustomDetachedGpgSignatures : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyVerifyCustomDetachedGpgSignatures(QObject *parent = 0);
private:
    QTextStream m_CustomDetachedSignaturesStream;
    RecursiveCustomDetachedSignatures *m_RecursiveCustomDetachedSignatures;
    QProcess *m_GpgProcess;
    QTextStream m_GpgProcessTextStream;
    QHash<QString /*file path*/, qint64 /*last modified unix timestamp in seconds*/> m_FilesOnFsToSeeIfAnyAreMissingSigsAndToCheckLastModifiedTImestampsAgainst;
    int m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash;
    RecursiveCustomDetachedSignaturesFileMeta m_FileMetaCurrentlyBeingVerified;

    quint64 m_NumFilesVerified;

    void buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(const QDir &dir, const QStringList &excludeEntries = QStringList());
    void verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore();
    bool readPathAndSignature(QString *out_FilePathToVerify, QString *out_CurrentFileSignature);
    void verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached();
    void spitOutGpgProcessOutput();
    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneRecursivelyVerifyCustomDetachedGpgSignatures(bool success);
public slots:
    void recursivelyVerifyCustomDetachedGpgSignatures(const QString &dir, const QString &customDetachedSignaturesFile /*TODOoptional: read from stdin*/, const QStringList &excludeEntries = QStringList());
    void recursivelyVerifyCustomDetachedGpgSignatures(const QDir &dir, QIODevice *customDetachedSignaturesIoDevice, const QStringList &excludeEntries = QStringList());
private slots:
    void handleGpgProcessError(QProcess::ProcessError processError);
    void handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURES_H
