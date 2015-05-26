#include <Wt/WLabel>
#include <Wt/WEnvironment>
using namespace Wt;
#include "documenttimelinewebwidget.h"

#include "documenttimelinesession.h"

IDocumentTimeline *DocumentTimelineWebWidget::s_DocumentTimeline = NULL;

DocumentTimelineWebWidget::DocumentTimelineWebWidget(const WEnvironment &environment, WtLibVersion version)
    : WApplication(environment, version)
{
    for(int i = 0; i < NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE; ++i)
    {
        DocumentTimelineDocWebWidget *newTimelineDocWebWidget = new DocumentTimelineDocWebWidget(root()); //TODOoptional: lazy create these 100 widgets
        new WBreak(root()); //TODOoptional: to go horizontal, I simply omit the WBreak
        m_DocumentTimelineDocsWidgets.insert(i, newTimelineDocWebWidget);
    }

    if(environment.ajax())
        this->enableUpdates(true);
    else
        this->deferRendering();

    DocumentTimelineSession::requestNewSession(s_DocumentTimeline, this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineSessionStarted, this, _1));
}
WApplication *DocumentTimelineWebWidget::documentTimelineWebWidgetEntryPoint(const WEnvironment &environment)
{
    return new DocumentTimelineWebWidget(environment);
}
void DocumentTimelineWebWidget::handleDocumentTimelineSessionStarted(DocumentTimelineSession *session)
{
    m_Session = session;
    m_Session->requestNewDocumentTimelineGetLatestDocuments(this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineGetLatestDocumentsFinished, this, _1, _2));
}
void DocumentTimelineWebWidget::handleDocumentTimelineGetLatestDocumentsFinished(bool getLatestTimelineDocsSuccess, QList<QByteArray> latestTimelineDocuments)
{
    if(environment().ajax())
        this->triggerUpdate();
    else
        this->resumeRendering();

    if(!getLatestTimelineDocsSuccess)
    {
        //TODOreq: modeless popup telling about the error
        return;
    }

    int max = NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE;
    int numDocs = qMin(latestTimelineDocuments.size(), max);
    for(int i = 0; i < numDocs; ++i)
    {
        //QDateTime docTimestamp = latestDocumentsTimestamps.at(i);
        //QString docSubmitterUsername = latestDocumentsUsernames.at(i);
        //QByteArray doc = latestDocumentsDatas.at(i);
        //QString docLicense = latestDocumentsLicenseIdentifiers.at(i);

        m_DocumentTimelineDocsWidgets.at(i)->setDoc(/*docTimestamp.toString().toStdString(), docSubmitterUsername.toStdString(), */latestTimelineDocuments.at(i)/*, docLicense.toStdString()*/);
    }
}
