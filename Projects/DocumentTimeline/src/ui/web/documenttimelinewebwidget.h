#include <Wt/WApplication>
using namespace Wt;
#ifndef DOCUMENTTIMELINEWEBWIDGET_H
#define DOCUMENTTIMELINEWEBWIDGET_H

#include <QStringList>

#include "documenttimelinedocwebwidget.h"

class IDocumentTimeline;
class DocumentTimelineSession;

class DocumentTimelineWebWidget : public WApplication
{
public:
    static IDocumentTimeline *s_DocumentTimeline;

    DocumentTimelineWebWidget(const WEnvironment &environment, WtLibVersion version = WT_INCLUDED_VERSION);
    static WApplication *documentTimelineWebWidgetEntryPoint(const WEnvironment &environment);
private:
    friend class DocumentTimelineWeb;
    DocumentTimelineSession *m_Session;

    QList<DocumentTimelineDocWebWidget*> m_DocumentTimelineDocsWidgets;

    void handleDocumentTimelineSessionStarted(DocumentTimelineSession *session);

    void handleDocumentTimelineGetLatestDocumentsFinished(bool getLatestTimelineDocsSuccess, QList<QByteArray> latestTimelineDocuments);
};

#endif // DOCUMENTTIMELINEWEBWIDGET_H
