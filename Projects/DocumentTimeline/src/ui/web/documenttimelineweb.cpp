#include "documenttimelineweb.h"

#include <QCoreApplication>

#include "documenttimeline.h"
#include "documenttimelinewebwidget.h"
#include "standardinputnotifier.h"

DocumentTimelineWeb::DocumentTimelineWeb(int argc, char *argv[], QObject *parent)
    : QObject(parent)
    , m_DocumentTimeline(new DocumentTimeline(this))
    , m_StdErr(stderr)
{
    connect(this, &DocumentTimelineWeb::exitRequested, qApp, &QCoreApplication::exit);

    StandardInputNotifier *standardInputNotifier = new StandardInputNotifier(this);
    connect(standardInputNotifier, &StandardInputNotifier::standardInputReceivedLine, this, &DocumentTimelineWeb::handleStandardInputLineReceived);

    QString firstArg = QCoreApplication::arguments().first();
    m_WtServer.reset(new WServer(firstArg.toStdString()));
    m_WtServer->setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    m_WtServer->addEntryPoint(Application, &DocumentTimelineWebWidget::documentTimelineWebWidgetEntryPoint);

    connect(m_DocumentTimeline, &DocumentTimeline::readyForSessions, this, &DocumentTimelineWeb::handleDocumentTimelineReadyForSessions);
    QMetaObject::invokeMethod(m_DocumentTimeline, "initializeAndStart");
}
DocumentTimelineWeb::~DocumentTimelineWeb()
{
    stopWtServer();
}
void DocumentTimelineWeb::stopWtServer()
{
    if(!m_WtServer.isNull())
    {
        if(m_WtServer->isRunning())
        {
            m_WtServer->stop();
        }
        m_WtServer.reset();
    }
}
void DocumentTimelineWeb::handleStandardInputLineReceived(const QString &standardInputLine)
{
    if(standardInputLine.toLower() == "q")
    {
        //TODOreq: let DocumentTimeline shutdown cleanly and async notify us before we do this:
        stopWtServer();
        emit exitRequested(0);
        return;
    }
    handleE("unknown user input: " + standardInputLine + "\nq = quit");
}
void DocumentTimelineWeb::handleE(QString msg)
{
    m_StdErr << msg << endl;
}
void DocumentTimelineWeb::handleDocumentTimelineReadyForSessions()
{
    if(m_WtServer.isNull() || m_WtServer->isRunning())
        return;
    DocumentTimelineWebWidget::s_DocumentTimeline = m_DocumentTimeline; //TODOoptional: I know there's a way to do it better than this (without a static), but I'm too lazy to do it atm
    m_WtServer->start();
}
