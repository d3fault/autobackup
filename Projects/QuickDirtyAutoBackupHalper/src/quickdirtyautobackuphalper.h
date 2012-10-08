#ifndef QUICKDIRTYAUTOBACKUPHALPER_H
#define QUICKDIRTYAUTOBACKUPHALPER_H

#include <QObject>
#include <QThread>
#include <QCoreApplication>

#include "quickdirtyautobackuphalperbusiness.h"
#include "mainwidget.h"

class QuickDirtyAutoBackupHalper : public QObject
{
    Q_OBJECT
public:
    explicit QuickDirtyAutoBackupHalper(QObject *parent = 0);
    void start();
private:
    QThread m_ThreadBusiness;
    QuickDirtyAutoBackupHalperBusiness m_Business;
    mainWidget m_Gui;
signals:
    
private slots:
    void handleAboutToQuit();
};

#endif // QUICKDIRTYAUTOBACKUPHALPER_H
