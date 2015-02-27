#ifndef VERIFYCUSTOMDETACHEDGPGSIGNATURESCLI_H
#define VERIFYCUSTOMDETACHEDGPGSIGNATURESCLI_H

#include <QObject>
#include <QTextStream>

class VerifyCustomDetachedGpgSignaturesCli : public QObject
{
    Q_OBJECT
public:
    explicit VerifyCustomDetachedGpgSignaturesCli(QObject *parent = 0);
private:
    QTextStream m_StdOut;
    QTextStream m_StdErr;
    bool m_Quitting;
signals:
    void exitRequested(int exitCode);
private slots:
    void handleO(const QString &msg);
    void handleE(const QString &msg);
    void handleDoneVerifyingCustomDetachedGpgSignatures(bool success);
};

#endif // VERIFYCUSTOMDETACHEDGPGSIGNATURESCLI_H
