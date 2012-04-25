#include "abcappdb.h"

AbcAppDb::AbcAppDb(QObject *parent) :
    QObject(parent), m_AppLogic(0)
{
}
void AbcAppDb::connectToBankServerAndStartListeningForWtFrontEnds()
{
    if(!m_AppLogic)
    {
        m_AppLogic = new AbcAppLogic();
        m_AppLogicThread = new QThread();
        m_AppLogic->moveToThread(m_AppLogicThread);
        m_AppLogicThread->start();

        m_BankServer = new BankServerHelper();
        m_BankServerThread = new QThread();
        m_BankServer->moveToThread(m_BankServerThread);
        m_BankServerThread->start();

        m_OurServerForWtFrontEnds = new OurServerForWtFrontEnds();
        m_OurServerForWtFrontEndsThread = new QThread();
        m_OurServerForWtFrontEnds->moveToThread(m_OurServerForWtFrontEndsThread);
        m_OurServerForWtFrontEndsThread->start();

        //connect bank server helper signals
        connect(m_BankServer, SIGNAL(connectedToBankServer()), m_OurServerForWtFrontEnds, SLOT(startListeningForWtFrontEnds()));
        connect(m_BankServer, SIGNAL(connectedToBankServer()), m_AppLogic, SLOT(init()));
        //^^we listen for the successful connection to the bank server to start up our other two classes
#ifdef ABC_CONNECTION_MANAGEMENT
        connect(m_BankServer, SIGNAL(disconnected()), m_OurServerForWtFrontEnds, SLOT(lostConnectionToBankServer())); //we don't need to stop listening for wt front ends, but any/most/all requests should now return 500 Internal Server Error. maybe we should also send this to AbcAppLogic? i can see race conditions where events are a) already at app logic, or even b) already coming to the bank server helper. we need to handle them all appropriately (500 Internal Server Error)
        connect(m_BankServer, SIGNAL(connectioniReEstablished()), m_OurServerForWtFrontEnds, SLOT(connectionToBankServerReEstablished())); //so we can continue normally. the idea is that m_BankServer will manage it's own connection... retrying if the connection is lost until it is re-gained.
#endif
        connect(m_BankServer, SIGNAL(responseToAppLogicReady(BankServerActionRequestResponse*)), m_AppLogic, SLOT(handleResponseFromBankServer(BankServerActionRequestResponse*)));

        //connect to our server signals. a lot of these signals have been are going to be handled in OurServerForWtFrontEnds... so they'll just be method calls inside there instead. the "request to app logic" signal is the only real signal
        //connect(m_OurServerForWtFrontEnds, SIGNAL(newWtFrontEndConnected()), m_AppLogic, SLOT(handleNewWtFrontEndConnected())); //send them the list of users that already have bank accounts, for example (but also remember who they are so we can push all updates to them). It might be better to keep AppLogic unaware of the connected wt front-ends. sure, we're going to forward the wt front-end identifier along with the 'GET'... but that's only so that the OurServerForWtFrontEnds knows who to send it to. AppLogic doesn't actually need the data. so basically OurServerForWtFrontEnds should have a copy of the list of users logged in and should send it whenever there's a newWtFrontEndConnection. no need to tell AppLogic
        //connect(m_OurServerForWtFrontEnds, SIGNAL(wtFrontEndExplicitlyDisconnected()), m_AppLogic, SLOT(handleWtFrontEndExplicitlyDisconnected())); //forget about them. again, why should AppLogic give a shit? the same goes for the wtFrontEndConnectionLost() below (although it's comments are still relevant... the code should just be in OurServerForWtFrontEnds... not in AbcAppLogic)
#ifdef ABC_CONNECTION_MANAGEMENT
        //connect(m_OurServerForWtFrontEnds, SIGNAL(wtFrontEndConnectionLost()), m_AppLogic, SLOT(handleWtFrontEndConnectionLost())); //stop pushing, or trying to push, updates to them. perhaps even just queue the updates (TODOreq: *INCLUDING PENDING/FAILING/ALREADY-SENT-TO-SOCKET*) to be sent later(?????????????????). The thing is... I expect connections to drop intermittently, and it is stupid to redo the entire 'first connection' process. stupid and wasteful (but then again... also simpler). so what i should do is just queue up all the data that would be sent on the socket in a bytearray. each time it is written to we see if it's passed a certain size. if it gets past a certain threshold, we purge the byte array and forget about the wt front-end client. the wt front-end has to be able to differentiate from a 'reconnect' (they then get all the data they missed), an initial connect (they get all the users), and a ***reconnect from being gone too long (extended connection issues). they will NOT be expecting a fresh user list, but will get one anyways. need to be prepared for that in the code. it should also purge their cache; they need to now assume it is all stale.
        //TODOopt: if a wt front-end can't connect to the appdb for an extended amount of time (or for any amount of time really), it would be nice to be able to do a redirect to one of the other wt front-end servers... but this implies that we'd be able to a) know their subdomain and b) know whether or not they too are offline/online. no point redirecting to a wt front-end that also can't connect to the appdb. this is an optimization that would require the servers to communicate with each other heavily
        //connect(m_OurServerForWtFrontEnds, SIGNAL(wtFrontEndConnectionReEstablished()), m_AppLogic, SLOT(handleWtFrontEndConnectionReEstablished())); //resume pushing updates... and maybe flush a cache of updates that they missed since their DC???
#endif
        connect(m_OurServerForWtFrontEnds, SIGNAL(requestFromWtFrontEnd(AppLogicRequest*)), m_AppLogic, SLOT(handleRequestFromWtFrontEnd(AppLogicRequest*))); //TODOreq: we *might* be able to share the protocol/struct 'AppLogicRequest' with the Wt-Front-End code for simplification/seemless integration. who knows.
        //TODOreq: should the AppLogicRequests be pointers? probably for optimization... but we also need to know when to clean them up (and/or recycle them (oh god don't get me started))

        //connect to our app logic's signals
        connect(m_AppLogic, SIGNAL(appLogicRequestRequiresBankServerAction(BankServerActionRequest*)), m_BankServer, SLOT(handleBankServerActionRequest(BankServerActionRequest*)));
        connect(m_AppLogic, SIGNAL(responseToWtFrontEndReady(AppLogicRequestResponse*)), m_OurServerForWtFrontEnds, SLOT(handleResponseFromAppLogic(AppLogicRequestResponse*)));

        //connect to all 3 back-end class's debug signals (as well as our own)
        connect(this, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
        connect(m_BankServer, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
        connect(m_AppLogic, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
        connect(m_OurServerForWtFrontEnds, SIGNAL(d(QString)), this, SLOT(handleD(QString)));

        //START CONNECTING TO THE BANK SERVER. AFTER THE CONENCTION IS ESTABLISHED, IT SIGNALS THE OTHER 2 TO START AS WELL
        QMetaObject::invokeMethod(m_BankServer, "connectToBankServer", Qt::QueuedConnection); //start asynchronously connecting
    }
}
void AbcAppDb::handleD(const QString &output)
{
    qDebug() << output;
}
