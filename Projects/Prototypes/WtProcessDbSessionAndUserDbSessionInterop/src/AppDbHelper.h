#ifndef APPDBHELPER_H
#define APPDBHELPER_H

#include <QObject>

//this singleton is one per Wt front-end. it ssl connects to the AppDb (one per "Wt Application" (not to be confused with user sessions. in the case of AdAgency, the AdAgency itself is my "Wt Application")
//there is no database in here... but accross the ssl connection is a db. that's the db that we are exposing the api for in this class. the appdb. the application logic. who has what website, what ad slot, how much ad slot is going for, ETC ETC

class AppDbHelper : public QObject
{
    Q_OBJECT
public:
    explicit AppDbHelper(QObject *parent = 0);

signals:

public slots:

};

#endif // APPDBHELPER_H
