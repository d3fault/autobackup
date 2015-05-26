#include <Wt/WServer>
using namespace Wt;
#ifndef DOCUMENTTIMELINEWEB_H
#define DOCUMENTTIMELINEWEB_H

#include <QObject>
#include <QSharedPointer>
#include <QTextStream>

class DocumentTimeline;

class DocumentTimelineWeb : public QObject
{
    Q_OBJECT
public:
    explicit DocumentTimelineWeb(int argc, char *argv[], QObject *parent = 0);
    virtual ~DocumentTimelineWeb();
private:
    DocumentTimeline *m_DocumentTimeline;
    QSharedPointer<WServer> m_WtServer;

    QTextStream m_StdErr;

    void stopWtServer();
signals:
    void exitRequested(int exitCode);
private slots:
    void handleStandardInputLineReceived(const QString &standardInputLine);
    void handleE(QString msg);
    void handleDocumentTimelineReadyForSessions();
};

#endif // DOCUMENTTIMELINEWEB_H
