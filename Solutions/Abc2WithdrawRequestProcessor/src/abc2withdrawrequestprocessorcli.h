#ifndef ABC2WITHDRAWREQUESTPROCESSORCLI_H
#define ABC2WITHDRAWREQUESTPROCESSORCLI_H

#include <QObject>

#include <QTextStream>

class Abc2WithdrawRequestProcessorCli : public QObject
{
    Q_OBJECT
public:
    explicit Abc2WithdrawRequestProcessorCli(QObject *parent = 0);
private:
    QTextStream m_StdErr;
    QTextStream m_StdOut;
private slots:
    void handleE(const QString &msg);
    void handleO(const QString &msg);
};

#endif // ABC2WITHDRAWREQUESTPROCESSORCLI_H
