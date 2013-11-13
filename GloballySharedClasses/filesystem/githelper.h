#ifndef GITHELPER_H
#define GITHELPER_H

#include <QObject>
#include <QProcess>

class GitHelper : public QObject
{
    Q_OBJECT
public:
    explicit GitHelper(QObject *parent = 0);
private:
    QProcess m_GitProcess;
signals:
    void d(const QString &);
public slots:
    void gitClone(QString srcRepoAbsolutePath, QString destRepoAbsolutePath, QString optionalWorkTree = QString());
};

#endif // GITHELPER_H
