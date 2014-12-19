#ifndef ABC2TRANSACTIONCREDITORCLI_H
#define ABC2TRANSACTIONCREDITORCLI_H

#include <QObject>
#include <QTextStream>

class Abc2TransactionCreditorCli : public QObject
{
    Q_OBJECT
public:
    explicit Abc2TransactionCreditorCli(QObject *parent = 0);
private:
    QTextStream m_StdErr;
    QTextStream m_StdOut;
private slots:
    void handleE(const QString &msg);
    void handleO(const QString &msg);
};

#endif // ABC2TRANSACTIONCREDITORCLI_H
