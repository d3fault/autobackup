#ifndef LOCALBANKDB_H
#define LOCALBANKDB_H

#include <QObject>
#include <QHash>

//in-memory prototype, the impl will be db/persistent... and look into using a QCache if it fits
class LocalBankDb : public QObject
{
    Q_OBJECT
public:
    explicit LocalBankDb(QObject *parent = 0);
    void addUser(QString newUser);
private:
          //user , balance
    QHash<QString, double> m_Db;
signals:

public slots:

};

#endif // LOCALBANKDB_H
