#include "qthttpclient.h"

#include <QtNetwork/QTcpSocket>

QtHttpClient::QtHttpClient(QTcpSocket *clientSocket, QObject *parent)
    : QObject(parent)
    //, m_ClientSocket(clientSocket)
    , m_ClientStream(clientSocket)
{
    m_ClientStream.setAutoDetectUnicode(true);
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
}
void QtHttpClient::handleReadyRead()
{
    if(m_ClientStream.device()->canReadLine())
    {
        QString currentLine = m_ClientStream.readLine();
        if(currentLine.startsWith("GET "))
        {

            m_ClientStream << "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=\"utf-8\"\r\n"
                              "\r\n"
                              "Your url: " << currentLine;
            m_ClientStream.device()->close();
        }
    }
}
