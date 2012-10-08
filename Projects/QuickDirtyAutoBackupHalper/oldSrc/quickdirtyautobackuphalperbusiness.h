#ifndef QUICKDIRTYAUTOBACKUPHALPERBUSINESS_H
#define QUICKDIRTYAUTOBACKUPHALPERBUSINESS_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>
#include <QFileInfo>
#include <QList>

#include "TcContainerPathAndPasswordPathValues.h"

class QuickDirtyAutoBackupHalperBusiness : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyAutoBackupHalperBusiness(QObject *parent = 0);
    QMutex *getWorkerMutex();
    bool isIdle();
    bool setDone(bool done);
private:
    QMutex m_WorkerMutex;
    bool m_MutexProtectedIdleFlag;
    bool m_MutexProtectedDoneFlag;

    void readSettingsAndProbeMountStatus();
    void probeMountStatus();
    bool existsAndIsntDirOrSymlink(QString filePath);
    QFileInfo m_FileInfo;

    QList<TcContainerPathAndPasswordPathValues> m_ReadInFromSettingsContainerAndPasswordPaths;

    QString m_StringTcLocationsAndPwFilesArrayName;
    QString m_StringContainerLocationKeyName;
    QString m_StringPasswordLocationKeyName;
signals:
    
public slots:
    void start();
    void mount();
};

#endif // QUICKDIRTYAUTOBACKUPHALPERBUSINESS_H
