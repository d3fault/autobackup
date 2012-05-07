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
public slots:
    void init();
};

#endif // BANK_H
