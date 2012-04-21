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
        connect(m_BankServer, SIGNAL(connected()), m_OurServerForWtFrontEnds, SLOT(startListeningForWtFrontEnds()));
        connect(m_BankServer, SIGNAL(disconnected()), m_OurServerForWtFrontEnds, SLOT(lostConnectionToBankServer())); //we don't need to stop listening for wt front ends, but any/most/all requests should now return 500 Internal Server Error. maybe we should also send this to AbcAppLogic? i can see race conditions where events are a) already at app logic, or even b) already coming to the bank server helper. we need to handle them all appropriately (500 Internal Server Error)
        connect(m_BankServer, SIGNAL(connectioniReEstablished()), m_OurServerForWtFrontEnds, SLOT(connectionToBankServerReEstablished())); //so we can continue normally. the idea is that m_BankServer will manage it's own connection... retrying if the connection is lost until it is re-gained.
        connect(m_BankServer, SIGNAL(responseToAppLogicReady(BankServerActionResponse)), m_AppLogic, SLOT(handleResponseFromBankServer(BankServerActionResponse)));


        //connect to our server signals
        connect(m_OurServerForWtFrontEnds, SIGNAL(newWtFrontEndConnected()), m_AppLogic, SLOT(handleNewWtFrontEndConnected())); //send them the list of users that already have bank accounts, for example (but also remember who they are so we can push all updates to them)
        connect(m_OurServerForWtFrontEnds, SIGNAL(wtFrontEndExplicitlyDisconnected()), m_AppLogic, SLOT(handleWtFrontEndExplicitlyDisconnected())); //forget about them
        connect(m_OurServerForWtFrontEnds, SIGNAL(wtFrontEndConnectionLost()), m_AppLogic, SLOT(handleWtFrontEndConnectionLost())); //stop pushing, or trying to push, updates to them. perhaps even just queue the updates (TODOreq: *INCLUDING PENDING/FAILING/ALREADY-SENT-TO-SOCKET*) to be sent later(?????????????????). The thing is... I expect connections to drop intermittently, and it is stupid to redo the entire 'first connection' process. stupid and wasteful (but then again... also simpler)
        connect(m_OurServerForWtFrontEnds, SIGNAL(wtFrontEndConnectionReEstablished()), m_AppLogic, SLOT(handleWtFrontEndConnectionReEstablished())); //resume pushing updates... and maybe flush a cache of updates that they missed since their DC???
        connect(m_OurServerForWtFrontEnds, SIGNAL(requestFromWtFrontEnd(AppLogicRequest)), m_AppLogic, SLOT(handleRequestFromWtFrontEnd(AppLogicRequest(AppLogicRequest)))); //TODOreq: we *might* be able to share the protocol/struct 'AppLogicRequest' with the Wt-Front-End code for simplification/seemless integration. who knows.
        //TODOreq: should the AppLogicRequests be pointers? probably for optimization... but we also need to know when to clean them up (and/or recycle them (oh god don't get me started))

        //connect to our app logic's signals
        connect(m_AppLogic, SIGNAL(appLogicRequestRequiresBankServerAction(BankServerAction)), m_BankServer, SLOT(handleBankServerActionRequest(BankServerAction)));
        connect(m_AppLogic, SIGNAL(responseToWtFrontEndReady(AppLogicRequestResponse)), m_OurServerForWtFrontEnds, SLOT(handleResponseFromAppLogic(AppLogicRequestResponse)));


        //START UP EACH COMPONENT AND VERIFY THAT THEY SUCCESSFULLY STARTED
        bool success = false;
        QMetaObject::invokeMethod(m_BankServer, "connect", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool,success));
        if(!success)
        {
            //TODO: abort(); or something... unable to connect to bank server is fatal errror
            return; //false? i could make main() return 1; right away if any of these fail... never getting to app.exec(); also TODO: if i emit d() and connect the d() to a qDebug() in the gui thread (this thread), we won't actually be able to process/output those until the BlockingQueuedConnection returns.. since it's/we're blocking lol. not sure if this matters. we're only blocking for these initial startup functions for each one
        }
        success = false;
        QMetaObject::invokeMethod(m_AppLogic, "init", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool,success));
        if(!success)
        {
            //TODO: abort(); or something, //only fails if SQL shit fails or something. not very likely
            return; //false?
        }
        success = false;
        QMetaObject::invokeMethod(m_OurServerForWtFrontEnds, "startListening", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool,success));
        if(!success)
        {
            //TODO: abort() or something... only reason i can think of why this wouldn't work is if the port/socket is already in use or something...
            return; //false?
        }
    }
}
