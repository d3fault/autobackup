#ifndef ABCAPPDB_H
#define ABCAPPDB_H

#include <QObject>
#include <QThread>

#include "abcapplogic.h"
#include "bankserverhelper.h"
#include "ourserverforwtfrontends.h"

class AbcAppDb : public QObject
{
    Q_OBJECT
public:
    explicit AbcAppDb(QObject *parent = 0);
    void connectToBankServerAndStartListeningForWtFrontEnds();
private:
    QThread *m_AppLogicThread;
    AbcAppLogic *m_AppLogic;

    QThread *m_BankServerThread;
    BankServerHelper *m_BankServer; //i could make a custom QSslSocket (still to be used as a client) that configures itself + accepts certificate data via constructor input... and re-use said QSslSocket (client!!!) in my Wt Front End code... by putting it in my /shared/ folder with all the other shiz. idk if it's worth it, but just a thought. SAVES LINES YO. saves lines, but does it save effort? maybe yes.. maybe no. idfk. stop writing about it though because THIS is definitely a waste... because it's a small as fuck optimization so stfu already~. ok continuing the thought, i might even be able to put the 'auto-reconnect' code in there too... which might make it more justifiable (and thus makes this paragraph more justifiable wooooooot don't tell me to stfu i'll tell you to stfu wait what who am i talking to. i guess whoever reads this source in [x] years. enjoy the lulz.)
    //i could do the same for the QSslSocket (server)... since AbcAppDb has a server for Wt Front Ends... and Bank Server has one too.
    //in any case, it can/should only be ssl+connection+auto-retry-connection logic that is re-used. there CANNOT be application/business logic of any kind otherwise i cannot share them.

    QThread *m_OurServerForWtFrontEndsThread;
    OurServerForWtFrontEnds *m_OurServerForWtFrontEnds;
signals:
public slots:
};

#endif // ABCAPPDB_H
