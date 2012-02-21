#include "protocolclient.h"

ProtocolClient::ProtocolClient(QObject *parent) :
    QObject(parent), m_Socket(0)
{
}
void ProtocolClient::connectToServer()
{
    if(!m_Socket)
    {
        m_Socket = new QTcpSocket(this);

        connect(m_Socket, SIGNAL(connected()), this, SLOT(handleConnected()));
        connect(m_Socket, SIGNAL(readyRead()), this, SLOT(handleServerReadyRead()));
        connect(m_Socket, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));

        m_Socket->connectToHost(QHostAddress::LocalHost, 6969);
        emit d("attempting to connect to server on port 6969");
    }
}
void ProtocolClient::handleConnected()
{
    emit d("connected");

    MessageFromClient helloMessage;
    helloMessage.m_MessageType = Message::MessageFromClientType; //this is redundant
    helloMessage.m_MessageSubType = MessageFromClient::HelloMessageFromClientSubType;
    helloMessage.m_MessageIntValue = 666;
    helloMessage.m_MessageStringValue = QString("what is the point of this during a hello message?");

    QDataStream stream(m_Socket);
    //stream << (*static_cast<const Message*>(&helloMessage)); //this line is fugly. oh wall
    stream << helloMessage;
    emit d("sending hello message to server");
}
void ProtocolClient::handleServerReadyRead()
{
    emit d("server ready read");
    QDataStream stream(m_Socket);
    while(!stream.atEnd())
    {
        emit d("top of atEnd while loop");

        MessageToClient messageToClient;
        stream >> messageToClient;
        if(messageToClient.m_MessageType != Message::MessageToClientType)
        {
            emit d("somehow received a message that wasn't a MessageToClientType");
            return;
        }
        switch(messageToClient.m_MessageSubType)
        {
        case MessageToClient::WelcomeMessageToClientSubType:
            emit d("welcome message received. how kind of them.");
            emit d("Number: " + QString::number(messageToClient.m_MessageIntValue));
            emit d(messageToClient.m_MessageStringValue);

            //thoughts:
            //a function to output/process the contents of Message, instead of doing it for each switch case
            //and a state machine for this class we are typing in. we can now put ourselves in STATE_CONNECTED etc... and listen to signals of the state changing and react appropriately (also other classes can listen to the signal and react appropriately)

            askForABeer();

            break;
        case MessageToClient::YesHereIsYourBeer:
            emit d("haha we just scammed the server out of a free beer. fuck yeeee");
            //sendThanks();
            //...etc...
            break;
        case MessageToClient::OkTakeCareMessageToClientSubType:
            emit d("server has responded to our goodbye saying 'take care'");
            break;
        case MessageToClient::InvalidMessageToClientSubType:
        default:
            emit d("uhh got a message and dunno wtf kind of subtype it is");
            break;
        }
    }
}
void ProtocolClient::handleDisconnected()
{
}
void ProtocolClient::askForABeer()
{
    MessageFromClient messageFromClient;
    messageFromClient.m_MessageType = Message::MessageFromClientType;
    messageFromClient.m_MessageSubType = MessageFromClient::MayIPleaseHaveABeer;
    //eh also gonna test what happens if i just leave the other two values blank. probably nothing as they have sensible defaults (TM COLON SLASH SRSLY HAVE HAD THE PHRASE 'sensible defaults' IN MY BRAIN FOR YEARS NOW)
    QDataStream stream(m_Socket);
    stream << messageFromClient;
    emit d("asking server for a beer");
}
