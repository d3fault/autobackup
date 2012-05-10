#ifndef BANK_H
#define BANK_H

#include <QObject>

class Bank : public QObject
{
    Q_OBJECT
public:
    explicit Bank(QObject *parent = 0);

signals:
    void initialized();
    void d(const QString &);
    void bankAccountCreated(const QString &username);
public slots:
    void init();
    void handleBankAccountCreationRequested(const QString &username);
    void handleBalanceTransferRequested(const QString &username, double amount);
};

#endif // BANK_H
