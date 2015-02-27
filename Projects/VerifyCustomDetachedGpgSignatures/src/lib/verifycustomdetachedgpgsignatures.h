#ifndef VERIFYCUSTOMDETACHEDGPGSIGNATURES_H
#define VERIFYCUSTOMDETACHEDGPGSIGNATURES_H

#include <QObject>
#include <QDir>
#include <QTextStream>
#include <QProcess>

class QIODevice;

class VerifyCustomDetachedGpgSignatures : public QObject
{
    Q_OBJECT
public:
    explicit VerifyCustomDetachedGpgSignatures(QObject *parent = 0);
private:
    QTextStream m_CustomDetachedSignaturesStream;
    QProcess *m_GpgProcess;
    QTextStream m_GpgProcessTextStream;

    void verifyNextEntryOfCustomDetachedOrEmitFinishedIfNoMore();
    bool readPathAndSignature(QString *out_FilePathToVerify, QString *out_CurrentFileSignature);
    void verifyFileSignatureAndThenContinueOntoNextEntryOfCustomDetached(const QString &filePathToVerify, const QString &fileSignature);
    void spitOutGpgProcessOutput();
signals:
    void o(const QString &msg);
    void e(const QString &msg);
    void doneVerifyingCustomDetachedGpgSignatures(bool success);
public slots:
    void verifyCustomDetachedGpgSignatures(const QString &dir, const QString &customDetachedSignaturesFile /*TODOoptional: read from stdin*/);
    void verifyCustomDetachedGpgSignatures(const QDir &dir, QIODevice *customDetachedSignaturesIoDevice);
private slots:
    void handleGpgProcessError(QProcess::ProcessError processError);
    void handleGpgProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // VERIFYCUSTOMDETACHEDGPGSIGNATURES_H
