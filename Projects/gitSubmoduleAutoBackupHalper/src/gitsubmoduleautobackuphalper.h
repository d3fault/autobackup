#ifndef GITSUBMODULEAUTOBACKUPHALPER_H
#define GITSUBMODULEAUTOBACKUPHALPER_H

#include <QObject>
#include <QCoreApplication>

#include "gitsubmoduleautobackuphalperwidget.h"
#include "gitsubmoduleautobackuphalperbusiness.h"
#include "objectonthreadhelper.h"

class GitSubmoduleAutoBackupHalper : public QObject
{
    Q_OBJECT
public:
    explicit GitSubmoduleAutoBackupHalper(QObject *parent = 0);
private:
    GitSubmoduleAutoBackupHalperWidget m_Gui;
    ObjectOnThreadHelper<GitSubmoduleAutoBackupHalperBusiness> m_Business;
public slots:
    void handleGitSubmoduleAutoBackupHalperBusinessInstantiated();
    void handleAboutToQuit();
};

#endif // GITSUBMODULEAUTOBACKUPHALPER_H
