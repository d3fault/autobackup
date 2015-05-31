#include <Wt/WLabel>
#include <Wt/WEnvironment>
#include <Wt/WAnchor>
#include <Wt/WText>
using namespace Wt;
#include "documenttimelinewebwidget.h"

#include "documenttimelinecommon.h"
#include "documenttimelineregisterwebdialogwidget.h"
#include "documenttimelineregistersubmitvideowidget.h"

IDocumentTimeline *DocumentTimelineWebWidget::s_DocumentTimeline = NULL;

DocumentTimelineWebWidget::DocumentTimelineWebWidget(const WEnvironment &environment, WtLibVersion version)
    : WApplication(environment, version)
    , m_RegisterWidget(NULL)
    , m_RegisterSubmitVideoWidget(NULL)
    , m_MessageBox(NULL)
{
    setCssTheme("polished");

    WText *documentTimelineTitle = new WText("<h2>Document Timeline</h2>", root());
    documentTimelineTitle->setFloatSide(Wt::Top | Wt::CenterX);

    WAnchor *registerAnchor = new WAnchor(WLink(WLink::InternalPath, DocumentTimelineWebWidget_INTERNAL_PATH_REGISTER), DocumentTimelineWebWidget_REGISTER, root());
    registerAnchor->setMargin(WLength(DocumentTimelineWebWidget_MARGIN_PX, WLength::Pixel));
    registerAnchor->setFloatSide(Wt::Top | Wt::Right);
    new WBreak(root());
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

    this->internalPathChanged().connect(this, &DocumentTimelineWebWidget::handleInternalPathChanged); //TODOreq: navigating to /register directly should work, but we don't have a DocumentTimeline session yet so.... ffff

    DocumentTimelineSession::requestNewSession(s_DocumentTimeline, this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineSessionStarted, this, _1));
}
DocumentTimelineWebWidget::~DocumentTimelineWebWidget()
{
    //if(m_Session)
        //delete m_Session; //TODOmb(have made session implicitly shared since writing this comment): have the business thread be the one to delete the session (since he instantiated it)? idfk tbh. a delete later call sounds safe though, I just hope I can easily make m_Session a QObject (plus doesn't making session a QObject imply that it is communicated with asynchronously (whereas currently, session is communicated with synchronously (it dispatches and that's it) (OT: i could make session async). It's not 'illegal' to do a synchronous QObject, but it's better to decide that all QObjects are asynchronous (because then synchronous is achieved via Qt::AutoConnection most of the time). The problem is: async objects with implicitly shared properties....... take longer to code. yes, 'session' is rpc generated in this case (WOOT), so it's not a problem for IT. but that is still a general programming problem. perhaps i need a standalone 'async [q]object with implicitly shared properties'... generator. heh but rpc generator (and even d=) is already that once I say it is. it is. it doesn't use signals for the async 'finished' emittance though (but could!). still, behind the request->respond() really is a signal emission if/when it's a Qt requester. fuck yea inheritence (*sucks own dick*). So am I saying that every session dispatch should be asynchronous just so it respects the QObject being inherited? One problem with that: I'm in Wt land. I am a fucking WApplication. I surely can't use rpc generator to communicate with the session, because then I've entered the chicken and the egg problem. So which is better(TODOreq:)? Making 'session' a QObject so I can call deleteLater on it, or passing 'session' back to business [thread] so he can do the deleting. both of them have the same effect of 'session is deleted on business thread', at least. I think I'm going to look into how 'login+session' will interact before deciding on this
}
WApplication *DocumentTimelineWebWidget::documentTimelineWebWidgetEntryPoint(const WEnvironment &environment)
{
    return new DocumentTimelineWebWidget(environment);
}
void DocumentTimelineWebWidget::handleInternalPathChanged(const std::string &newInternalPath)
{
    if(m_Session.isNull())
        return;
    if(newInternalPath == DocumentTimelineWebWidget_INTERNAL_PATH_REGISTER)
    {
        m_RegisterWidget.reset(new DocumentTimelineRegisterWebDialogWidget());
        m_RegisterWidget->finished().connect(this, &DocumentTimelineWebWidget::handleRegisterWidgetFinished);
        m_RegisterWidget->show();
        return;
    }
}
void DocumentTimelineWebWidget::handleDocumentTimelineSessionStarted(DocumentTimelineSession session)
{
    m_Session.reset(new DocumentTimelineSession(session));
    m_Session->requestNewDocumentTimelineGetLatestDocuments(this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineGetLatestDocumentsFinished, this, _1, _2, _3));
}
void DocumentTimelineWebWidget::requestPending()
{
    if(!environment().ajax())
        this->deferRendering();
}
void DocumentTimelineWebWidget::responseReceived()
{
    if(environment().ajax())
        this->triggerUpdate();
    else
        this->resumeRendering();
}
void DocumentTimelineWebWidget::setMessageBoxMessage(const WString &caption, const WString &text)
{
    m_MessageBox.reset(new WMessageBox());
    m_MessageBox->setModal(false);
    m_MessageBox->setCaption(caption);
    m_MessageBox->setText(text);
    m_MessageBox->finished().connect(this, &DocumentTimelineWebWidget::handleMessageBoxFinished);
    m_MessageBox->show();
}
void DocumentTimelineWebWidget::handleMessageBoxFinished(WDialog::DialogCode dialogCode)
{
    Q_UNUSED(dialogCode)
    m_MessageBox.reset();
}
void DocumentTimelineWebWidget::handleRegisterWidgetFinished(WDialog::DialogCode dialogCode)
{
    if(m_RegisterWidget.isNull())
        return;
    if(dialogCode == WDialog::Accepted)
    {
        m_Session->requestNewDocumentTimelineDeclareIntentToAttemptRegistration(this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineDeclareIntentToAttemptRegistrationFinished, this, _1, _2, _3), m_RegisterWidget->fullName(), m_RegisterWidget->desiredUsername(), m_RegisterWidget->password(), m_RegisterWidget->acceptedClaCheckbox(), m_RegisterWidget->fullNameSignature());
        requestPending();
    }
    m_RegisterWidget.reset();
}
void DocumentTimelineWebWidget::handleRegisterSubmitVideoWidgetFinished(WDialog::DialogCode dialogCode)
{
    if(m_RegisterSubmitVideoWidget.isNull())
        return;
    if(dialogCode == WDialog::Accepted)
    {
        m_Session->requestNewDocumentTimelineSubmitRegistrationAttemptVideo(this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineSubmitRegistrationAttemptVideoFinished, this, _1, _2), m_RegisterSubmitVideoWidget->desiredUsername(), m_RegisterSubmitVideoWidget->password(), m_RegisterSubmitVideoWidget->filePathOfJustUploadedRegistrationAttemptVideo());
        requestPending();
    }
    m_RegisterSubmitVideoWidget.reset();
}
void DocumentTimelineWebWidget::handleDocumentTimelineGetLatestDocumentsFinished(bool internalError, bool getLatestTimelineDocsSuccess, QList<QByteArray> latestTimelineDocuments)
{
    responseReceived();

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
void DocumentTimelineWebWidget::handleDocumentTimelineDeclareIntentToAttemptRegistrationFinished(bool internalError, bool intentToRegisterDeclarationSuccess, QString dataUserMustReciteInRegistrationAttemptVideo)
{
    responseReceived();

    if(!intentToRegisterDeclarationSuccess)
    {
        setMessageBoxMessage("Failed to submit registration details", "Something went wrong trying to submit the registration details. Maybe the username is already taken, maybe the db has crashed"); //TODOreq: dbError and lcbOpSuccess
        return;
    }
    m_RegisterSubmitVideoWidget.reset(new DocumentTimelineRegisterSubmitVideoWidget(dataUserMustReciteInRegistrationAttemptVideo));
    m_RegisterSubmitVideoWidget->finished().connect(this, &DocumentTimelineWebWidget::handleRegisterSubmitVideoWidgetFinished);
    m_RegisterSubmitVideoWidget->show();
    return;
}
void DocumentTimelineWebWidget::handleDocumentTimelineSubmitRegistrationAttemptVideoFinished(bool internalError, bool registrationAttemptVideoSubmissionSuccess)
{
    responseReceived();


}
