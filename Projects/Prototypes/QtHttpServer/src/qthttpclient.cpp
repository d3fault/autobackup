#include "qthttpclient.h"

#include <QtNetwork/QTcpSocket>
#include <QUrl>
#include <QUrlQuery>

QtHttpClient::QtHttpClient(QTcpSocket *clientSocket, QObject *parent)
    : QObject(parent)
    //, m_ClientSocket(clientSocket)
    , m_ClientStream(clientSocket)
{
    m_ClientStream.setAutoDetectUnicode(true);
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
void QtHttpClient::handleReadyRead()
{
    QString response("<html><body>");
    while(m_ClientStream.device()->canReadLine())
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
    }
    response.append("</body></html>");
    m_ClientStream << "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=\"utf-8\"\r\nConnection: close\r\nContent-Length: " << QString::number(response.length()) << "\r\n"
                      "\r\n"
                   << response;
    m_ClientStream.device()->close();
}
