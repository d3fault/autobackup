#ifndef QUICKDIRTYAUTOBACKUPHALPER_H
#define QUICKDIRTYAUTOBACKUPHALPER_H

#include <QObject>
#include <QProcess>

class QuickDirtyAutoBackupHalper : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyAutoBackupHalper(QObject *parent = 0);
    ~QuickDirtyAutoBackupHalper();
private:
    enum WtfAmIDoing
    {
        DoingNothingOrInvalid = 0,
        ProbingMountStatus,
        MountingTruecrypt,
        DismountingTruecrypt,
        TreeingDirStructure,
        Committing
        //shutting down has no use for this
    };

    WtfAmIDoing m_WtfAmIDoing;

    QProcess *m_MySubProcess;
    //bool m_ProcIsTruecrypt;
    bool m_OneErrorWasDetected;
    QString *m_AllProcessOutput;
    QString currentProcessNameHumanReadable();
    void processTheOutputBasedOnWhatImDoing();
signals:
    void d(const QString &);
    void initializedAndProbed(bool fucked, bool theyAreMounted);
    void mounted();
    void dismounted();
    void committed();
public slots:
    void initializeAndProbe();
    void mount(bool mountAsReadOnly);
    void dismount();
    void commit(const QString &commitMsg);
    //void shutdown();
    bool isBusyWithSomething();
private slots:
    void handleProcessError(QProcess::ProcessError error);
    void handleProcessStarted();
    //void handleProcessReadyRead();
    void handleProcessFinished(int exitCode,QProcess::ExitStatus exitStatus);
    void handleProcessStateChanged(QProcess::ProcessState newState);
};

#endif // QUICKDIRTYAUTOBACKUPHALPER_H
