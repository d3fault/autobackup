#ifndef RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURES_H
#define RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURES_H

#include <QObject>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QSet>

class QIODevice;

class RecursiveCustomDetachedSignatures;

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
    QSet<QString> m_ListOfFileOnFsToSeeIfAnyAreMissingSigs;
    int m_CharacterLengthOfAbsolutePathOfTargetDir_IncludingTrailingSlash;
    QString m_FilePathCurrentlyBeingVerified;

    quint64 m_NumFilesVerified;

    void buildListOfFilesOnFsToSeeIfAnyAreMissingSigs(const QDir &dir);
    void verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore();
    bool readPathAndSignature(QString *out_FilePathToVerify, QString *out_CurrentFileSignature);
    void verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(const QString &filePathToVerify, const QString &fileSignature);
    void spitOutGpgProcessOutput();
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneRecursivelyVerifyCustomDetachedGpgSignatures(bool success);
public slots:
    void recursivelyVerifyCustomDetachedGpgSignatures(const QString &dir, const QString &customDetachedSignaturesFile /*TODOoptional: read from stdin*/);
    void recursivelyVerifyCustomDetachedGpgSignatures(const QDir &dir, QIODevice *customDetachedSignaturesIoDevice);
private slots:
    void handleGpgProcessError(QProcess::ProcessError processError);
    void handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURES_H
