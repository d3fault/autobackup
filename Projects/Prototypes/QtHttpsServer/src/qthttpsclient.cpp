#include "qthttpsclient.h"

#include <QtNetwork/QSslSocket>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

QtHttpsClient::QtHttpsClient(QSslSocket *clientSocket, QObject *parent)
    : QObject(parent)
    //, m_ClientSocket(clientSocket)
    , m_ClientStream(clientSocket)
{
    m_ClientStream.setAutoDetectUnicode(true);
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
    connect(clientSocket, SIGNAL(peerVerifyError(QSslError)), this, SLOT(handlePeerVerifyError(QSslError)));
    connect(clientSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
    connect(clientSocket, SIGNAL(encrypted()), this, SLOT(handleClientSocketEncrypted()));
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
void QtHttpsClient::handleError(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
}
void QtHttpsClient::handlePeerVerifyError(QSslError sslError)
{
    qDebug() << sslError;
}
void QtHttpsClient::handleSslErrors(QList<QSslError> sslErrors)
{
    qDebug() << sslErrors;
}
void QtHttpsClient::handleClientSocketEncrypted()
{
    connect(m_ClientStream.device(), SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
}
void QtHttpsClient::handleReadyRead()
{
    QString response("<html><body>");
    if(m_ClientStream.device()->canReadLine())
    {
        QString currentLine = m_ClientStream.readLine();
        response.append("Your Request: " + currentLine + "<br />\n");
        if(currentLine.startsWith("GET "))
        {
            QStringList getLineParts = currentLine.split(" ");
            if(getLineParts.size() >= 2)
            {
                QString encodedUrl = getLineParts.at(1);
                if(encodedUrl.startsWith("/"))
                {
                    QUrl theUrl = QUrl::fromUserInput("http://dummydomain.com" + encodedUrl);
                    if(theUrl.isValid())
                    {
                        response.append("Your url, decoded: " + theUrl.path() + "<br />\n");

                        QUrlQuery theUrlQuery(theUrl);
                        QString xStr = theUrlQuery.queryItemValue("x");
                        if(!xStr.isEmpty())
                        {
                            bool parseSuccess = false;
                            int x = xStr.toInt(&parseSuccess);
                            if(parseSuccess)
                            {
                                response.append("The value of x: " + QString::number(x) + "<br />\n");
                            }
                        }
                    }
                }
            }
        }
        response.append("</body></html>");
        m_ClientStream << "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=\"utf-8\"\r\nConnection: close\r\nContent-Length: " << QString::number(response.length()) << "\r\n"
                          "\r\n"
                       << response;
        m_ClientStream.flush(); //for some reason, flushing is mandatory else the browser just ehh hangs there. 10/10 luck/instincts calling this, i was so close to giving up...
        static_cast<QAbstractSocket*>(m_ClientStream.device())->flush();
        m_ClientStream.device()->close(); //OLD (solution was to flush the abstract socket AS WELL): for some reason, if you close the socket here, it doesn't fucking work (worked just fine with htttp server)
    }
    //else: readyRead was emitted multiple times before i could read a line, when the browser was the one initiating the request
}
