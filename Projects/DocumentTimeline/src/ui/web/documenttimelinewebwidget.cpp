#include <Wt/WLabel>
#include <Wt/WEnvironment>
#include <Wt/WAnchor>
using namespace Wt;
#include "documenttimelinewebwidget.h"

#include "documenttimelinecommon.h"
#include "documenttimelinesession.h"
#include "documenttimelineregisterwebdialogwidget.h"

IDocumentTimeline *DocumentTimelineWebWidget::s_DocumentTimeline = NULL;

DocumentTimelineWebWidget::DocumentTimelineWebWidget(const WEnvironment &environment, WtLibVersion version)
    : WApplication(environment, version)
    , m_Session(NULL)
    , m_RegisterWidget(NULL)
    , m_MessageBox(NULL)
{
    WAnchor *registerAnchor = new WAnchor(WLink(WLink::InternalPath, DocumentTimelineWebWidget_INTERNAL_PATH_REGISTER), DocumentTimelineWebWidget_REGISTER, root());
    registerAnchor->setMargin(WLength(DocumentTimelineWebWidget_MARGIN_PX, WLength::Pixel));
    registerAnchor->setFloatSide(Wt::Right);
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

    this->internalPathChanged().connect(this, &DocumentTimelineWebWidget::handleInternalPathChanged);

    DocumentTimelineSession::requestNewSession(s_DocumentTimeline, this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineSessionStarted, this, _1));
}
DocumentTimelineWebWidget::~DocumentTimelineWebWidget()
{
    if(m_Session)
        delete m_Session; //TODOmb: have the business thread be the one to delete the session?
    deletMessageBoxIfInstantiated();
}
WApplication *DocumentTimelineWebWidget::documentTimelineWebWidgetEntryPoint(const WEnvironment &environment)
{
    return new DocumentTimelineWebWidget(environment);
}
void DocumentTimelineWebWidget::handleInternalPathChanged(const std::string &newInternalPath)
{
    if(!m_Session)
        return;
    if(newInternalPath == DocumentTimelineWebWidget_INTERNAL_PATH_REGISTER)
    {
        if(m_RegisterWidget)
            delete m_RegisterWidget;
        m_RegisterWidget = new DocumentTimelineRegisterWebDialogWidget();
        m_RegisterWidget->finished().connect(this, &DocumentTimelineWebWidget::handleRegisterWidgetFinished);
        return;
    }
}
void DocumentTimelineWebWidget::handleDocumentTimelineSessionStarted(DocumentTimelineSession *session)
{
    if(m_Session)
        delete m_Session;
    m_Session = session;
    m_Session->requestNewDocumentTimelineGetLatestDocuments(this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineGetLatestDocumentsFinished, this, _1, _2));
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
    if(m_MessageBox)
        delete m_MessageBox;
    m_MessageBox = new WMessageBox();
    m_MessageBox->setModal(false);
    m_MessageBox->setCaption(caption);
    m_MessageBox->setText(text);
    m_MessageBox->finished().connect(this, &DocumentTimelineWebWidget::handleMessageBoxFinished);
}
void DocumentTimelineWebWidget::handleMessageBoxFinished(WDialog::DialogCode dialogCode)
{
    deletMessageBoxIfInstantiated();
}
void DocumentTimelineWebWidget::deletMessageBoxIfInstantiated()
{
    if(m_MessageBox)
    {
        delete m_MessageBox;
        m_MessageBox = NULL;
    }
}
void DocumentTimelineWebWidget::handleRegisterWidgetFinished(WDialog::DialogCode dialogCode)
{
    if(!m_RegisterWidget)
        return;
    if(dialogCode == WDialog::Accepted)
    {
        m_Session->requestNewDocumentTimelineDeclareIntentToAttemptRegistration(this->sessionId(), boost::bind(&DocumentTimelineWebWidget::handleDocumentTimelineDeclareIntentToAttemptRegistrationFinished, this, _1, _2), m_RegisterWidget->fullName(), m_RegisterWidget->desiredUsername(), m_RegisterWidget->password(), m_RegisterWidget->acceptedClaCheckbox(), m_RegisterWidget->fullNameSignature());
    }
    delete m_RegisterWidget;
    m_RegisterWidget = NULL;

    requestPending();
}
void DocumentTimelineWebWidget::handleDocumentTimelineGetLatestDocumentsFinished(bool getLatestTimelineDocsSuccess, QList<QByteArray> latestTimelineDocuments)
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
void DocumentTimelineWebWidget::handleDocumentTimelineDeclareIntentToAttemptRegistrationFinished(bool intentToRegisterDeclarationSuccess, QString dataUserMustReciteInRegistrationAttemptVideo)
{
    responseReceived();

    if(!intentToRegisterDeclarationSuccess)
    {
        setMessageBoxMessage("Failed to submit registration details", "Something went wrong trying to submit the registration details. Maybe the username is already taken, maybe the db has crashed"); //TODOreq: dbError and lcbOpSuccess
        return;
    }
    //TODOreq: submit registration video form
}
