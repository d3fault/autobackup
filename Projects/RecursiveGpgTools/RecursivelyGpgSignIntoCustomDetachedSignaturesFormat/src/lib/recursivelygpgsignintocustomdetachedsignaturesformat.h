#ifndef RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMAT_H
#define RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMAT_H

#include <QObject>
#include <QTextStream>
#include <QHash>
#include <QDirIterator>
#include <QScopedPointer>
#include <QProcess>

#include "recursivecustomdetachedgpgsignatures.h"

class QIODevice;

class RecursivelyGpgSignIntoCustomDetachedSignaturesFormat : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyGpgSignIntoCustomDetachedSignaturesFormat(QObject *parent = 0);
private:
    QTextStream m_InputSigsFileTextStream;
    QIODevice *m_OutputSigsFile;
    QHash<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem;
    QMap<QString /*file path*/, RecursiveCustomDetachedSignaturesFileMeta /*file meta*/> m_AllSigsForOutputting;
    RecursiveCustomDetachedSignatures *m_RecursiveCustomDetachedSignatures;
    int m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash;
    QScopedPointer<QDirIterator> m_DirIterator;
    QProcess *m_GpgProcess;
    QTextStream m_GpgProcessTextStream;
    RecursiveCustomDetachedSignaturesFileMeta m_FileMetaCurrentlyBeingGpgSigned;
    QStringList m_ExcludeEntries;

    quint64 m_ExistingSigs;
    quint64 m_AddedSigs;
    quint64 m_TotalSigs;

    void readInAllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem();
    void recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore();
    void gpgSignFileAndThenContinueRecursingDir();
    void spitOutGpgProcessOutput();
    static inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
            return inputString;
        return inputString + "/";
    }
signals:
    void e(const QString &msg);
    void doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool success);
public slots:
    void recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QString &dirToRecursivelySign, const QString &sigFilePath_OrEmptyStringForStdOut, const QStringList &excludeEntries = QStringList());
    void recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QDir &dirToRecursivelySign, QIODevice *sigsIoDevice, const QStringList &excludeEntries = QStringList());
    void recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QDir &dirToRecursivelySign, QIODevice *inputSigsIoDevice, QIODevice *outputSigsIoDevice, const QStringList &excludeEntries = QStringList());
private slots:
    void handleGpgProcessError(QProcess::ProcessError processError);
    void handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMAT_H
