#include "ssltcpserver.h"

SslTcpServer::SslTcpServer(QObject *parent) :
    QTcpServer(parent), m_ServerPrivateEncryptionKey(0), m_ServerPublicLocalCertificate(0)
{ }
SslTcpServer::~SslTcpServer()
{
    if(isListening())
    {
        this->stop();
    }
    if(m_ServerPrivateEncryptionKey)
    {
        delete m_ServerPrivateEncryptionKey;
    }
    if(m_ServerPublicLocalCertificate)
    {
        delete m_ServerPublicLocalCertificate;
    }
}
void SslTcpServer::initialize(SslTcpServerArgs sslTcpArgs)
{
    if(!QSslSocket::supportsSsl())
    {
        emit d("ssl is not supported");
        return;
    }
    m_SslTcpServerArgs = sslTcpArgs;

    //Server CA is what the client's use to verify the authenticity of the server. It is a "List", but in our case only contains our server's Local Certificate. It has to be 'securely' copied to the client connection. both sides have it set. On this side, we only use it when loading our public local certificate to see if it's on the list (and verify against it).
    QFile serverCaFileResource(sslTcpArgs.ServerCaFilename);
    serverCaFileResource.open(QFile::ReadOnly);
    QByteArray serverCaByteArray = serverCaFileResource.readAll();
    serverCaFileResource.close();

    QSslCertificate serverCA(serverCaByteArray);
    if(serverCA.isNull())
    {
        emit d("the server CA is null");
        return;
    }
    emit d("the server CA is not null");
    if(!serverCA.isValid())
    {
        emit d("the server CA is not valid");
        return;
    }
    emit d("the server CA is valid");

    if(sslTcpArgs.UseClientCA2WaySecurity)
    {
        //Client CA is a list of certificates that the server uses to verify the authenticity of the clients. each client can use the same certificate, but for our purposes we're going to have each client having a unique certificate. it is how we will identify them.
        QFile clientCaFileResource(sslTcpArgs.ClientCaFilename);
        clientCaFileResource.open(QFile::ReadOnly);
        QByteArray clientCaByteArray = clientCaFileResource.readAll();
        clientCaFileResource.close();

        QSslCertificate clientCA(clientCaByteArray);
        if(clientCA.isNull())
        {
            emit d("client CA is null");
            return;
        }
        emit d("client CA is not null");
        if(!clientCA.isValid())
        {
            emit d("client CA is not valid");
            return;
        }
        emit d("client CA is valid");

        m_AllMyCertificateAuthorities.append(clientCA);
    }

    m_AllMyCertificateAuthorities.append(serverCA);

    //Server Private Encryption Key is the private portion of our Local Certificate
    QByteArray serverPrivateEncryptionKeyPassPhraseByteArray(sslTcpArgs.ServerPrivateEncryptionKeyPassPhrase.toUtf8()); //TODO: not sure if .toUtf8() is right... even though it does in fact return a QByteArray. idfk what kind it expects.. shit doesn't say

    QFile serverPrivateEncryptionKeyFileResource(sslTcpArgs.ServerPrivateEncryptionKeyFilename);
    serverPrivateEncryptionKeyFileResource.open(QFile::ReadOnly);
    QByteArray serverPrivateEncryptionKeyByteArray = serverPrivateEncryptionKeyFileResource.readAll();
    serverCaFileResource.close();

    m_ServerPrivateEncryptionKey = new QSslKey(serverPrivateEncryptionKeyByteArray, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, serverPrivateEncryptionKeyPassPhraseByteArray);

    if(m_ServerPrivateEncryptionKey->isNull())
    {
        emit d("server private encryption key is null");
        return;
    }
    emit d("server private encryption key is not null");

    //Server Public Local Certificate is the public decryption key that we send to our client
    QFile serverPublicLocalCertificateFileResource(sslTcpArgs.ServerPublicLocalCertificateFilename);
    serverPublicLocalCertificateFileResource.open(QFile::ReadOnly);
    QByteArray serverPublicLocalCertificateByteArray = serverPublicLocalCertificateFileResource.readAll();
    serverPublicLocalCertificateFileResource.close();

    m_ServerPublicLocalCertificate = new QSslCertificate(serverPublicLocalCertificateByteArray);
    if(m_ServerPublicLocalCertificate->isNull())
    {
        emit d("server public local certificate is null");
        return;
    }
    emit d("server public local certificate is not null");
    if(!m_ServerPublicLocalCertificate->isValid())
    {
        emit d("server public local certificate is not valid");
        return;
    }
    emit d("server public local certificate is valid");

    //TODOreq: using/overriding the default configuration like this means that i don't need to do it on the client if i do it on the server and vice versa. in fact, there is now an issue with the CA certificates... but the issue will only arise if you use client/server in the same project (unlikely, but i've done it before in tests and such). same with the private key. the server has a private key and the client has a private key. for them to see each other's, and especially to USE each other's... will fuck shit up. perhaps i should move this code back into the handlenewConnectionNotYetEncrypted... though it is a slightl 'optimization' to put it here. Things like VerifyPeer are not going to be used in EVERY project anyways (though probably will for most of mine). etc etc.
    //for the client it only makes sense to use override the 'default configuration' in the constructor anyways. each 'init' is a new socket... so it'd be setting the default configuration over and over and over (whereas server is only initialized once). but then of course by moving it to the constructor (BACK* to the constructor i should say ;-P), i lose the ability to emit signals etc telling me if the certs etc are all valid and shit. it is often that typos etc make my certs invalid
    //on the subject of emitting in constructors. one way to do it would be to pass in some 'interface' all the time that can be called to emit debug shit. it's sloppy hack and will uglify my code, but meh, would work. fuckin Qt with not having signals in constructors gah. i realize what i'm saying may appear strange at first, but then NOT having signals in the constructor is actually inconsistent with the rest of the code blocks. even the destructor can emit signals (TODOoptional: wtf can a sender() be null by the time a queued message is received... say if the signal is dispatched in the destructor... but even if it's just delete'd moments/lines later. i think yes. weird case i've never thought up TODOreq: make sure anywhere you use sender() that you check it is still alive)
    //An example of where the defaults will conflict is when using Rpc Generator as both a client and a Service (two unrelated ones). AbcAppDb... or my 'cache' layer... idfk what it is at this point... may... or may not... function as both. There are many other unforseen/invented scenarios where an application runs as both an Rpc Client and Rpc Server.
    //set up ssl defaults for all future connections
    sslTcpArgs.SslConfiguration.setSslOption(QSsl::SslOptionDisableCompression, true); //as per CRIME
    sslTcpArgs.SslConfiguration.setCaCertificates(m_AllMyCertificateAuthorities);
    sslTcpArgs.SslConfiguration.setPrivateKey(*m_ServerPrivateEncryptionKey);
    sslTcpArgs.SslConfiguration.setLocalCertificate(*m_ServerPublicLocalCertificate);
    if(sslTcpArgs.UseClientCA2WaySecurity)
    {
        sslTcpArgs.SslConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer); //makes us request + REQUIRE that the client cert is valid. client does this to us by default, but we don't do it to him by default. i should still explicitly set it for client
    //TODO: perhaps whether or not to use PeerVerify should be a constructor bool that is saved and applied to each incoming connection. the server should still be usable in non-peer-verify mode
    }
    else
    {
        sslTcpArgs.SslConfiguration.setPeerVerifyMode(QSslSocket::AutoVerifyPeer);
    }

    QSslConfiguration::setDefaultConfiguration(sslTcpArgs.SslConfiguration);

    connect(this, SIGNAL(newConnection()), this, SLOT(handleNewConnectionNotYetEncrypted()));

    emit d("ssl tcp server initialized");
    return;
}
void SslTcpServer::start()
{
    if(this->listen(m_SslTcpServerArgs.HostAddress, m_SslTcpServerArgs.Port))
    {
        emit d(QString("now listening for ssl tcp connections on ") + m_SslTcpServerArgs.HostAddress.toString() + QString(":") + QString::number(m_SslTcpServerArgs.Port));
    }
    else
    {
        emit d("ssl tcp server listen failed");
    }
}
void SslTcpServer::stop()
{
    this->close(); //TODOreq: close() doesn't do shit for existing connections, but might disallow future ones? i need to keep track of connected clients and then call QSslSocket::disconnectFromHost on them to disconnect cleanly
    emit d("stopping ssl tcp server");
}
void SslTcpServer::incomingConnection(int handle)
{
    QSslSocket *secureSocket = new QSslSocket(this);
    if(secureSocket->setSocketDescriptor(handle))
    {
        m_PendingConnections.enqueue(secureSocket);

        secureSocket->startServerEncryption();

        emit d("server is initiating delayed handshake");

        /*
        if(!secureSocket->waitForEncrypted())
        {
            emit d(secureSocket->errorString());
        }
        emit d("got past wait");*/
    }
    else
    {
        emit d("failed to set socket descriptor");
        delete secureSocket;
    }
}
QTcpSocket *SslTcpServer::nextPendingConnection()
{
    if(m_PendingConnections.isEmpty())
    {
        return 0;
    }
    return m_PendingConnections.dequeue();
}
void SslTcpServer::handleNewConnectionNotYetEncrypted()
{
    emit d("new connection, not yet encrypted");
    QSslSocket *newConnection = qobject_cast<QSslSocket*>(this->nextPendingConnection());
    if(newConnection)
    {
        connect(newConnection, SIGNAL(encrypted()), this, SLOT(handleConnectedAndEncrypted()));
        connect(newConnection, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>))); //TODOreq: The user of this class should really handle it, but I mean I could always make it auto-disconnect whenever an error is seen?
        connect(newConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError))); //TODOreq: ditto ------ HOWEVER all they really do is provide debug info anyways so I'll just leave them for now
        emit d("got a new connection from nextPendingConnection()");
    }
    else
    {
        emit d("nextPendingConnection() returned 0");
    }
}
void SslTcpServer::handleConnectedAndEncrypted()
{
    emit d("connection ENCRYPTED");
    QSslSocket *secureSocket = qobject_cast<QSslSocket*>(sender());
    if(secureSocket)
    {
        //I don't know what I want to use for the clientId. It's a pretty simple (FAMOUS_LAST_WORD) problem, yes, but I do need to keep in mind that I want Rpc Generator to perhaps someday function accross threads/processes(?) as well. QLocalSocket for same-machine Rpc Services. SOOOO things like Ip Address and Ssl Cert Serial # are useless. PERHAPS the client _SENDS_ me his client ID upon connection establishment? And Does that mean he sends it with every message too???? Sounds quite wasteful, but perhaps necessary'ish?
        //I like the idea of the server issuing a clientId upon connection, BUT then how will he know if the client has already connected? The client providing his own clientId helps greatly in detecting stale/actually-disconnected connections (and then we update them with the new/valid socket :-P)
        //I looked at QLocalSocket and QAbstractSocket... and they don't have any matching methods ('drop in QLocalSocket replacement') for identifying the client. SOOOOO it looks like the most portable/future-proof way is to have the client report his own ID. Still not sure if he needs to send it with every message... but really I don't THINK that should be necessary. Premature optimization much? IDFK tbh. Overhead is overhead. Let's pro/con this shit!

        //---ClientId sent during 'hello' only
        //pro: less protocol overhead per-message

        //---ClientId sent with every message
        //con: more protocol overhead per-message

        //ok that didn't fucking help at all rofl...

        //what I'm wondering though is like, with the 'hello' method, when receiving a message, the socket pointer will be consistent. OK. Then we look up the ClientId that we stored during 'hello' and write it to the message. We find it by looking it up _BY_ the socket pointer (TODOreq: we should never even get to that code segment until a successful 'hello'). After the business processes it, we do the opposite: get the socket pointer by clientId. we do primitive tests to see if it's still online (LOL that statement took me so many weeks to understand), then send it. If we detect using primitive tests that the connection isn't good (or if during sending a boolean returns false telling us [with greater certainty] that the connection is gone), then we queue that shit up. TODOreq: however if we cleanly disconnect (TODOreq: clean disconnects should _WAIT_ for messages in business imo) in the meantime, there is no reason to queue. So I guess that parenthesis TODOreq negates the TODOreq that came right before it? If you don't wait, you haven't cleanly disconnected! (and then we queue~). TODOreq: no clue how long I should queue for, but it makes sense that we'd eventually give up and that a reconnect would be so fucking 'out of whack/sync' that there is no point in sending the queue'd shit anyways. BUT for the 1-second disconnect/reconnect case, it makes sense to queue dat shiz. Man this logic is getting confusing, I feel like I need a diagram of use cases that involves clean/dirty disconnects, as well as reconnects. Lots more use cases than I admit. I'm getting away from the point though... ClientIds
        //vs
        //there is nothing really special/complicated/hard-to-understand-or-grasp about sending the clientId with every message. We don't have to depend on the socket pointer to look up a clientId... however we do need to save the socket pointer by clientId somehow then? Hmmm now I'm definitely leaning towards that 'hello' method.

        //FALSE: BOTH CASES REQUIRE THE SAME AMOUNT OF MEMORY/LOOKUPS, 'hello' method requires less network protocol overhead (valuable!). 'hello' does require less bandwidth overhead, but the non-hello method does not have to look up the clientId by the socket pointer for each message... so it's one less lookup. fuck it.

        //they both have to look up the socket pointer by clientId, of course. that is the connection-independent functionality that I desire.

        //I do wonder if all of this shit, the 'hello' and 'clean-disconnect' + intermediate 'ready-for-action' states should be put into another class that that 'hasA' 'this'. IDK tbh.
        //HOLY SHIT I KNOW ONE THING: I'm in ssl-specific code... so yea it probably definitely should go in another class lmfao. One that allows me to use Tcp Sockets for LAN connections (no ssl required), ssl sockets for WAN connections, and QLocalSocket for same-machine connections. Each of those use the hello/ready/cleanDisconnectPlz states ON TOP OF the underlying socket. And the client reports his own clientId during hello. I'm still unsure how clean disconnects will work. Maybe the client just puts himself in a "don't send more to this server" mode and then just waits and then sends a dc message (or maybe none at all... just a tcp disconnect???). Maybe the server rejects new messages for that clientId once we enter cleanDisconnectPlz? I guess it could be either, but having the client do it sounds like the better solution/design. I'm probably goign to remove the existing ClientId code from this class, which'll probably break other code (inb4 git submodules :-()

        //another solution is to just derive from qtcpsocket/qsslsocket/qlocalsocket AS WELL AS some interface with a pure virtual 'uniqueClientId', and just let each implement it however. But meh. IDK. Is that better? The same? Which is easier? Are their cons of it that I'm just not seeing?

        //TODOreq: think about the possibility of an Rpc Service on a machine that can talk to all 3 of those kinds of sockets. Holy shit my brain just exploded. It's definitely plausible that you'd have a qlocalsocket/same-machine client, a tcp/LAN client, and an ssl/WAN client all at the same time. I guess I should choose the solution that accomodates for that crazy/awesome hypothetical use case. ROBUST AS FUCK YESPLZKTHXBAI (WHO CARES IF I LOSE MY SANITY AND OPTIMIZE MYSELF INTO NON-EXISTENCE)


        //ok my brain just asploded
        //and I _MIGHT_ have found an argument for having QLocalSocket derive from the same base as qtcp/udp/etc
        //like err yea they only need to be QIODevice for streaming in/out
        //but what the fuck, are connect/disconnect the same?
        //gah all of this is easily avoided if i have each of them implement a special pure virtual interface
        //but i also don't want to O_o?
        //idfk.

        //DROP IN REPLACEMENT MY ASS
        //QLocalSocket has 'connectToServer', QAbstractSocket has 'connectToHost', which also has a port specifier -_-

        //So basically... my actual 'server' should be apathetic/unaware of what kind of sockets we are listening on. It should emit QIODevices* (or maybe a special interface I create?) after it does the verifying of 'hello'.

        //TODOreq: if the server gets the same client id, we should also check that it really is the same? I might accidentally use the same client id for a localsocket and an sslsocket for example. The server should be able to detect this (or even if they are the same type: both sslsockets for example) and then be all like "invalid client id motherfucker". When the client receives that he should report it to some error reporting utility that is sent directly to my email or whatever. A place where high priority errors are reported (for example, hack attempts are NOT reported here (but if somehow I manage to detect a successfull hack (without being able to simultaneously prevent it? unlikely), that should be reported to the high priority error email addy as well)). <-- this is an argument in favor of having the server provide clientIds during 'hello'. I guess the client would just have to memorize the clientId and know to send it during the re-connect phase. If the machine goes offline or whatever (aside from being far too our of sync) we will have lost the ClientId and have to get another. Hmmmmmmm decisions/designs/thinking/etc fml.
        //currently I'm thinking the ClientId would be hardcoded, but the ClientId could also be like a fucking md5(mac+rand()+timestamp+salt) and generated on app startup....
        //TODOidfk: ^^relating to comment just above, perhaps a persistent-between-machine-reboot permaClientId would be valuable in letting the server know that "hey, that queue you are saving for me is way the fuck outdated so just get rid of it lol". BUT then I now run into the same problem of "same perma client id as some other client" ROOOOOFL my brain~)

#ifdef ABC_CONNECTION_MANAGEMENT
        uint clientSerialNumber = secureSocket->peerCertificate().serialNumber().toUInt();
        if(m_EncryptedSocketsBySerialNumber.contains(clientSerialNumber))
        {
            //TODOreq: they've already connected... which i guess would mean that they lost connection and are now reconnecting... and we didn't detect it and remove them. TODOreq: we need to detect it and remove them (also). This would be some sort of check during the sending phase I suppose? A bool that some shit returns will become false? Should test/verify this
            //TODOreq: deal with the un-flushed queue of shit we thought we sent or something or whatever idfk. don't just forget
            m_EncryptedSocketsBySerialNumber[clientSerialNumber] = secureSocket; //for now, just overwrite the old socket
        }
        else
        {
            //not in our hash yet, so add it
            m_EncryptedSocketsBySerialNumber.insert(clientSerialNumber, secureSocket);
            emit clientConnectedAndEncrypted(clientSerialNumber, secureSocket);
        }
#endif
        //this might be used later in connection management, but right now i just want getSocketByUniqueId(uint id);
        //m_SocketsByUniqueId
        //lol found out i already had one from commented out connection management attempt
        m_EncryptedSocketsBySerialNumber.insert(getClientUniqueId(secureSocket), secureSocket);
        //i wonder if it's  more efficient to just store teh socket. this way at least i ensure only 1 per cert (don't i?)
        //also, do I need an authentication/identification scheme on top of the certs????? idfk, but am going to use one anyways

        emit clientConnectedAndEncrypted(secureSocket);
    }
}
void SslTcpServer::handleSslErrors(QList<QSslError> sslErrors)
{
    int numErrors = sslErrors.count();
    emit d(QString::number(numErrors) + " ssl errors");
    for(int i = 0; i < numErrors; ++i)
    {
        emit d("ssl error #" + QString::number(i) + " - " + sslErrors.at(i).errorString());
    }
}
void SslTcpServer::handleSocketError(QAbstractSocket::SocketError abstractSocketError)
{
    emit d("abstract socket error #" + QString::number(abstractSocketError));
}
uint SslTcpServer::getClientUniqueId(QSslSocket *client)
{
    return client->peerCertificate().serialNumber().toUInt(); //so i can change this for ip address or something easily if i need to
}
QSslSocket *SslTcpServer::getSocketByUniqueId(uint uniqueId)
{
    return m_EncryptedSocketsBySerialNumber.value(uniqueId);
}
QList<uint> SslTcpServer::getAllConnectedUniqueIds()
{
    return m_EncryptedSocketsBySerialNumber.keys();
}
bool SslTcpServer::isSslConnectionGood(QSslSocket *socketToCheck)
{
    return ( ( socketToCheck->isValid() ) && ( socketToCheck->state() == QAbstractSocket::ConnectedState ) && ( socketToCheck->isEncrypted() ) ); //This is NOT a tcp ack guarantee ;-)
}
