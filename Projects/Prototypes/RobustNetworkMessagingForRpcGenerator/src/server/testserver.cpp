#include "testserver.h"

void TestServer::init()
{
    m_DebugMessageNum = 0;
    m_ShitIsFucked = false;
    m_Server = new SslTcpServer(this, ":/ServerCA.pem", ":/ClientCA.pem", ":/ServerPrivateKey.pem", ":/ServerPublicCert.pem", "fuckyou");

    connect(m_Server, SIGNAL(clientConnectedAndEncrypted(QSslSocket*)), this, SLOT(handleClientConnectedAndEncrypted(QSslSocket*)));
    connect(m_Server, SIGNAL(d(QString)), this, SIGNAL(d(QString)));

    if(m_Server->init())
    {
        emit initialized();
    }
    else
    {
        emit d("server failed to initialize");
    }
}
void TestServer::start()
{
    if(m_Server->start())
    {
        emit started();
        emit d("server started");
    }
    else
    {
        emit d("server failed to start");
    }
}
void TestServer::stop()
{
    if(SslTcpServer::isSslConnectionGood(m_Client))
    {
        m_Client->disconnectFromHost();
        m_Server->stop();
    }
    emit stopped();
}
void TestServer::handleClientConnectedAndEncrypted(QSslSocket *newClientSocket)
{
    emit d("a client connected");
    connect(newClientSocket, SIGNAL(readyRead()), this, SLOT(handleDataReceivedFromClient()));
    m_Client = newClientSocket;
    m_ClientDataStream = new QDataStream(m_Client);
}
void TestServer::handleDataReceivedFromClient()
{
    emit d("data received from client");

    while(!m_ClientDataStream->atEnd())
    {
        QString text;
        (*m_ClientDataStream) >> text;
        emit d(text);
    }
}
void TestServer::sendMessageToPeer()
{
    if(m_Client && !m_ShitIsFucked/*&& SslTcpServer::isSslConnectionGood(m_Client)*/)
    {
        QString blah;
        blah.append(QDateTime::currentDateTime().toString());
        blah.append(QString(" - "));
        blah.append(QString::number(m_DebugMessageNum));
        blah.append(QString("\n"));

        emit d(QString("Sending Message Num: ") + QString::number(m_DebugMessageNum));

        ++m_DebugMessageNum;

        (*m_ClientDataStream) << blah;

        /* -- ok nvm this didn't do jack shit. it probably only responds to QIODevice::close(); i got the AbstractError #1 and it kept sending. i commented out the if(m_Client && isSslConnectionGood) statement for the test, because AbstractError #1 does at least make SslConnectionGood return false so we don't continue. HOWEVER now i have the idea of checking that the connection is good AFTER i write to it using QDataStream. it will hopefully have the same effect. that way there isn't a race condition where i check the ssl connection is good, then it becomes bad before i can send. it's only a few lines down, but it's a possibility!! if i were to detect that the connection is bad, having the 'previously attempted to send' message available would be very nifty as then i can start queueing the messages :). how i currently have it coded means the previously sent message (that we JUST would have figured out failed) is long gone. it existed in this slot in a different context.

          ROBUST AS FUCK BUT LESS OPTIMIZED: check connection is good both before and after :-P... though i doubt writing to a failed socket ONCE before we realize it's failed is that big of a deal. HOWEVER, there's also the race condition that it DID get sent!! that sslGood is true while we send but then becomes false before we can test it. a solution to this is somewhat easy however... when we re-establish the connection i can say "here's this message. we tried to send it. it MIGHT have made it (race condition), but probably did not" -- we only send that for the first-new (last-old) message. the one that was sent before we detected sslGood is false. this is actually probably the most robust solution, with minimal overlap. i could code more to test this... in fact i probably should (and i need to physically pull the ethernet cable out between two comps! then quickly put it back in and see what happens). i do still think asking how to errrrr verify a QDataStream write is a good question for Qt interest.... even though i'm not at Qt 5 head so bah a bad test. in fact i think this fedora 17 distro is using 4.8 beta rofl

          does sslConnectionGood guarantee that all previous messages were sent? the above design assumes yes

          TODOreq: i mean of course i should still do a check on sslConnectionGood in some other initialization stage or something... not just [before?] after each message is sent

        if(m_ClientDataStream->status() != QDataStream::Ok)
        {
            emit d("write motherfucking failed. at least i found out how to detect it :)");
        }
        */


        if(!SslTcpServer::isSslConnectionGood(m_Client))
        {
            m_ShitIsFucked = true;
            emit d("connection is no longer good, here's what was sent last to the QIODevice. it needs to be queued for next connection");
            emit d(blah);
        }
    }
}
