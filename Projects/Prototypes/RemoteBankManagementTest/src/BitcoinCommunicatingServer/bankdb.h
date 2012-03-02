#ifndef BANKDB_H
#define BANKDB_H

#include <QObject>
#include <QHash>

//in-memory prototype. impl will be database backed/persistent
class BankDb : public QObject
{
    Q_OBJECT
public:
    explicit BankDb(QObject *parent = 0);
    void addUser(const QString &appId, const QString &userName); //todo: should either return a bool or an AddUserResult. to make this initial prototype easier and to get it up and running, we're going to assume it worked... because it will
private:
    /*
      --- our 'local app bank db' reciprocal ---
      we need to take into account the appId
          //user , balance
    QHash<QString, qint64> m_Db;
    */

          //appId        //user   //balance
    QHash<QString, QHash<QString, double> > m_Db;
signals:

public slots:

};

#endif // BANKDB_H
