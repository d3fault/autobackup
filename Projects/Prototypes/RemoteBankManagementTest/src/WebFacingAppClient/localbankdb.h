#ifndef LOCALBANKDB_H
#define LOCALBANKDB_H

#include <QObject>
#include <QHash>

class LocalBankDb : public QObject
{
    Q_OBJECT
public:
    explicit LocalBankDb(QObject *parent = 0);
    void addUser(QString newUser);
private:
          //user , balance
    QHash<QString, double> *m_Db;
signals:

public slots:

};

#endif // LOCALBANKDB_H
