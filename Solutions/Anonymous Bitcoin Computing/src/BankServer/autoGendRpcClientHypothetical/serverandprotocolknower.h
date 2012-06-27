#ifndef SERVERANDPROTOCOLKNOWER_H
#define SERVERANDPROTOCOLKNOWER_H

#include <QObject>
#include <QMultiHash>
#include <QString>

#include "../shared/ssltcpserver.h"
#include "../RpcBankServer/RpcBankServerNetworkProtocol.h"

//weirdly, this class has the exact opposite interface implementation as the regular rpc implementations. it's auto-generated so it doesn't matter.. it's just funny i guess?. all signals in the interface become slots here, all slots in interface become signals here. will be fun to write the generator code for that lol
class ServerAndProtocolKnower : public QObject
{
    Q_OBJECT
public:
    explicit ServerAndProtocolKnower(QObject *parent = 0);
private:
    SslTcpServer *m_SslTcpServer;

    //QList<QString /* TODO: object generation if there's more than one parameter? */> m_PendingCreateBankAccountRequests; //to the rpc bank impl
    QMultiHash<uint,QString> m_NetworkClientIdAndPendingCreateBankAccountRequestsHash; //could also just do all pending requests, but we're using a generator and seeing as we're going to be looking through these for dupes it is also efficient to have one per request type
signals:
    void d(const QString &);
    void createBankAccount(const QString &);
public slots:
    void createBankAccountFailedUsernameAlreadyExists(CreateBankAccountMessage *createBankAccountMessage);
    void createBankAccountFailedPersistError(CreateBankAccountMessage *createBankAccountMessage);
    void createBankAccountCompleted(CreateBankAccountMessage *createBankAccountMessage); //really, the response (this here slot) doesn't need the username. we just need a magic cookie to identify the requests back up with the responses... and then using the request handle (would this be an ASyncMb* or is this a different subject?) that we connect signals to, we do sender() when the response is received and now have the request... and therefore the string.
    //as far as errors, we could auto-generate an enum that says specifically which parameter failed or something. idfk. the rpc server impl would have to set them... which makes sense

    //as far as the appdb -> bank server request holding + identifying with response
    //AND ALSO THE RECYCLING OF SAID RequestResponse
    //...in AppLogic we get a new/cached one... and when the message comes back to AppLogic, it puts it back in the queue/list to be re-used
    //BUT, this breaks down on the server side i think
    //it also doesn't make sense for broadcasts. they do not have a request. i guess that's ok as it is a special RpcBankServerRequest* that we listen to. think about how QNetworkAccessManager works. we want to listen to a signal directly off of QNetworkAccessManager to receive broadcasts... and listen to a signal from RpcBankServerRequest* (if async?) for requests/their-respective-responses

    //anywyas, back to how it breaks down on the server
    //it's because the signal emitted by the auto-gen'd rpc clientshelper (really, THIS CLASS RIGHT HERE) on the server side is not able to match up responses (slot) to it's requests (signal)
    //since server has two entry points (oh wait, so does appdb): logic + network
    //for both, the network encapsulation/thread needs to be the only one accessing the caching of messages etc
    //ok it doesn't break on the server. the broadcast is emitted to us directly and we take care of the message allocation. since it's a broadcast slot (signal in the interface) and auto-generated, we know to get a new/reuse-a-previous (not to be confused with "reUse an old", which is what responses will do with their request message)
    //ok i lied, it does break on the server. never does the appdb rpc client emit the extracted values of the request like server does to the rpc impl
    //i guess we can add the request to a list of pending requests (each action can have it's own list so as to improve performance) and we just do parameter checking for matching back up. again, broadcasts don't factor in. maybe all this message re-use bullshit is a waste of time. i really don't know... but it still definitely does make sense to re-use
    //so appdb/rpc-client uses magic cookie to identify response over network.... and [auto-generated] rpc-bank-server uses action-specific QLists of pending request (TODOreq: should probably add it to the list before emitting the extracted members... JUST IN CASE it's a direct connection. actually it doesn't matter if it is, because it has to emit the fact that it finished anyways (although, THAT could be a direct call too... back to us... and BAM, yes, we do need to put it in the list before doing the emit)
    //the only delay now is a lookup comparing against all the currently requested/pending Actions by their parameters (just username for CreateBankAccount (TODO: should we also check that it isn't already pending before emit'ing the request? i guess that's one of our race condition detectors)) while we wait
    //TODOopt: there is an order of operations for which variables to evaluate against each other first when checking the pending list. the OOO is known by the rpc generator. it is the one that chooses what to use as a key... assuming it's a hash.... or at the very least, which of the variables to compare FOR EQUALITY first
    //i'm really almost talking about an object generated for each Action, that contains, as members, the parameter types
    //sure, CreateBankAccount(QString) just needs a QList<QString> pendingCreateBankAccounts.... but for anything with more than one parameter, we now need an object to be the type in the QList.
    //SEEING AS we're generating an Object for each Action, it might be wise to re-think the design so as to incorporate/use said objects. up to and including on the server auto-generated code. oh wait that's what we're talking about. up to and includng the rpc client serverhelper code. hell, the protocol even....
private slots:
    void handleClientConnectedAndEncrypted(QSslSocket *client);
    void handleMessageReceivedFromRpcClientOverNetwork();
};

#endif // SERVERANDPROTOCOLKNOWER_H
