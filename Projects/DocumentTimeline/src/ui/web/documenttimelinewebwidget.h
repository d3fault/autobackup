#include <Wt/WApplication>
#include <Wt/WDialog>
#include <Wt/WServer>
#include <Wt/WMessageBox>
using namespace Wt;
#ifndef DOCUMENTTIMELINEWEBWIDGET_H
#define DOCUMENTTIMELINEWEBWIDGET_H

#include <QStringList>

#include "documenttimelinedocwebwidget.h"

class IDocumentTimeline;
class DocumentTimelineSession;
class DocumentTimelineRegisterWebDialogWidget;
class DocumentTimelineRegisterSubmitVideoWidget;

class DocumentTimelineWebWidget : public WApplication
{
public:
    static IDocumentTimeline *s_DocumentTimeline;

    DocumentTimelineWebWidget(const WEnvironment &environment, WtLibVersion version = WT_INCLUDED_VERSION);
    ~DocumentTimelineWebWidget();
    static WApplication *documentTimelineWebWidgetEntryPoint(const WEnvironment &environment);
    void handleInternalPathChanged(const std::string &newInternalPath);
private:
    friend class DocumentTimelineWeb;
    DocumentTimelineSession *m_Session;
    DocumentTimelineRegisterWebDialogWidget *m_RegisterWidget;
    DocumentTimelineRegisterSubmitVideoWidget *m_RegisterSubmitVideoWidget;

    WMessageBox *m_MessageBox;

    QList<DocumentTimelineDocWebWidget*> m_DocumentTimelineDocsWidgets;

    void handleDocumentTimelineSessionStarted(DocumentTimelineSession *session);

    void requestPending();
    void responseReceived();

    void setMessageBoxMessage(const WString &caption, const WString &text);
    void handleMessageBoxFinished(WDialog::DialogCode dialogCode);

    void deleteRegisterWidgetIfInstantiated();
    void deleteRegisterSubmitVideoWidgetIfInstantiated();
    void deletMessageBoxIfInstantiated();

    void handleRegisterWidgetFinished(WDialog::DialogCode dialogCode);

    void handleDocumentTimelineGetLatestDocumentsFinished(bool getLatestTimelineDocsSuccess, QList<QByteArray> latestTimelineDocuments);
    void handleDocumentTimelineDeclareIntentToAttemptRegistrationFinished(bool intentToRegisterDeclarationSuccess, QString dataUserMustReciteInRegistrationAttemptVideo);
};

#endif // DOCUMENTTIMELINEWEBWIDGET_H
