#ifndef GITSUBMODULEAUTOBACKUPHALPERBUSINESS_H
#define GITSUBMODULEAUTOBACKUPHALPERBUSINESS_H

#include <QObject>

class GitSubmoduleAutoBackupHalperBusiness : public QObject
{
    Q_OBJECT
public:
    explicit GitSubmoduleAutoBackupHalperBusiness(QObject *parent = 0);
    
signals:
    void d(const QString &);
public slots:
    
};

#endif // GITSUBMODULEAUTOBACKUPHALPERBUSINESS_H
