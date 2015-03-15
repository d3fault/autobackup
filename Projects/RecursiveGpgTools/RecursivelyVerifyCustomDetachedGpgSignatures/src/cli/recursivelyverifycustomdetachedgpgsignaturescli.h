#ifndef RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURESCLI_H
#define RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURESCLI_H

#include <QObject>
#include <QTextStream>

class RecursivelyVerifyCustomDetachedGpgSignaturesCli : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyVerifyCustomDetachedGpgSignaturesCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;
    bool m_Quitting;
signals:
    void exitRequested(int exitCode);
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleDoneRecursivelyVerifyCustomDetachedGpgSignatures(bool success);
};

#endif // RECURSIVELYVERIFYCUSTOMDETACHEDGPGSIGNATURESCLI_H
