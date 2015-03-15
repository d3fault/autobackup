#ifndef RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMAT_H
#define RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMAT_H

#include <QObject>
#include <QTextStream>
#include <QHash>
#include <QDirIterator>
#include <QScopedPointer>
#include <QProcess>

class RecursiveCustomDetachedSignatures;

class RecursivelyGpgSignIntoCustomDetachedSignaturesFormat : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyGpgSignIntoCustomDetachedSignaturesFormat(QObject *parent = 0);
private:
    QTextStream m_InputAndOutputSigFileTextStream;
    QHash<QString /*file path*/, QString /*file sig*/> m_AllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem;
    QMap<QString /*file path*/, QString /*file sig*/> m_AllSigsForOutputting;
    RecursiveCustomDetachedSignatures *m_RecursiveCustomDetachedSignatures;
    int m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash;
    QScopedPointer<QDirIterator> m_DirIterator;
    QProcess *m_GpgProcess;
    QTextStream m_GpgProcessTextStream;
    QString m_FilePathCurrentlyBeingGpgSigned;

    quint64 m_ExistingSigs;
    quint64 m_AddedSigs;
    quint64 m_TotalSigs;

    void readInAllSigsFromSigFileSoWeKnowWhichOnesToSkipAsWeRecurseTheFilesystem();
    void recursivelyGpgSignDirEntriesAndEmitFinishedWhenNoMore();
    void gpgSignFileAndThenContinueRecursingDir(const QString &filePathToGpgSign);
    void spitOutGpgProcessOutput();
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneRecursivelyGpgSigningIntoCustomDetachedSignaturesFormat(bool success);
public slots:
    void recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QString &dirToRecursivelySign, const QString &outputSigFilePath, bool forceResigningOfFilesAlreadyPresentInOutputSigFile = false);
    void recursivelyGpgSignIntoCustomDetachedSignaturesFormat(const QDir &dirToRecursivelySign, QIODevice *outputSigIoDevice, bool forceResigningOfFilesAlreadyPresentInOutputSigFile = false);
private slots:
    void handleGpgProcessError(QProcess::ProcessError processError);
    void handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // RECURSIVELYGPGSIGNINTOCUSTOMDETACHEDSIGNATURESFORMAT_H
