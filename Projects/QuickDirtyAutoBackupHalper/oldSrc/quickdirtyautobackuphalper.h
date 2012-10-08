#ifndef QUICKDIRTYAUTOBACKUPHALPER_H
#define QUICKDIRTYAUTOBACKUPHALPER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include "quickdirtyautobackuphalperbusiness.h"
#include "quickdirtyautobackuphalperwidget.h"

class QuickDirtyAutoBackupHalper : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyAutoBackupHalper(QObject *parent = 0);
    void start();
private:
    QuickDirtyAutoBackupHalperBusiness m_Business;
    QuickDirtyAutoBackupHalperWidget m_Gui;

    QThread m_ThreadBusiness;
    QMutex *m_MutexBusiness; //hack but easier coding and safety. this isn't speed critical code anyways
signals:
    
private slots:
    bool canCloseBecauseBusinessIsIdleAndWeStoppedItOk();
};

#endif // QUICKDIRTYAUTOBACKUPHALPER_H
