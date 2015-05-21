#include "cleanroomwidget.h"

#include <QLineEdit>

#include "cleanroomsession.h"
#include "cleanroomguiresponder.h"
#include "cleanroomfrontpagedefaultviewrequestfromqt.h"

CleanRoomWidget::CleanRoomWidget(QWidget *parent)
    : QWidget(parent)
    , m_Responder(new CleanRoomGuiResponder(this, SLOT(handleCleanRoomSessionStarted(CleanRoomSession*)), SLOT(cleanRoomApiCallback(ICleanRoomSessionRequest*))))
    //, CleanRoomSession(m_CleanRoom,m_Responder))
{ }
ICleanRoomFrontEndResponder *CleanRoomWidget::responder() const
{
    return m_Responder;
}
void CleanRoomWidget::handleRegistrationAttemptFinished(ICleanRoomSessionRequest *request)
{
    //RegisterSessionRequest *requestAsActual = static_cast<RegisterSessionRequest*>(request);
    //setWindowTitle(tr("Registration attempt: " + (requestAsActual->response()->success() ? "success" : "fail")));
}
void CleanRoomWidget::handleRegisterButtonClicked()
{
    //QString username, password; //TODOreq
    ///*emit */sessionRequestRequested(createRegisterSessionRequest(m_RegisterUsernameLineEdit->text(), m_RegisterPasswordLineEdit->text(), SLOT(handleRegistrationAttemptFinished(ICleanRoomSessionRequest*)))); //TODOreq: sanitize the line edits. maybe move where they live to a 'register' specific object?
}
void CleanRoomWidget::handleLoginButtonClicked()
{

}
void CleanRoomWidget::handleSubmitButtonClicked()
{
    //QByteArray data; //TODOreq
    ///*emit */sessionRequestRequested(createSubmitDataSessionRequest(data));
}
void CleanRoomWidget::handleLogoutButtonClicked()
{

}
void CleanRoomWidget::handleCleanRoomSessionStarted(CleanRoomSession *session)
{
    m_Session = session;
    QSharedPointer<CleanRoomFrontPageDefaultViewRequestFromQt> *frontPageDefaultViewRequest(new CleanRoomFrontPageDefaultViewRequestFromQt(m_Session, SLOT(handleFrontPageDefaultViewReceived(QList<QString>))));
    //Wt neighbor impl uses:
    //QSharedPointer<CleanRoomFrontPageDefaultViewRequestFromWt> *frontPageDefaultViewRequest(new CleanRoomFrontPageDefaultViewRequestFromWt(m_Session, boost::bind(&CleanRoomWtWidget::handleFrontPageDefaultViewReceived, _1))); <-- or something, syntax might be off. the _1 should be the frontPageDocs

    //i was considering this "instantiate type to begin api call" design vs. an m_Session->requestCleanRoomFrontPageDefaultViewRequestFromQt(...callback...) but the way I'm doing it now decouples the session with the front ends I think. Man so many ways to skin this cat and it's hard to implement an elegant/KISS one only because it's hard to decide
}
void CleanRoomWidget::cleanRoomApiCallback(ICleanRoomSessionRequest *request)
{
    //we're now back in the front-end context/thread
    //so it's simply a matter of calling the right slot at this point... except gah see huge design flaws. idfk how to represent a callback that works both with wt and qt. another flaw is that since the 'default view front end' was done automatically, we never got to provide a callback lewl. i feel like i should have my responder be the one that both jumps the thread/context and ALSO performs the callback... however the problem with that boils down to: variable amounts of method arguments per api call. i `kinda` have it solved with QVariantList, but idk I feel like I'm missing something. like does it come down to a switch statement on the first entry in the variantlist in order to re-determine the api callback and then have hardcoded right there the extraction of the api callback arguments? seems ugly and not very callback'ish. perhaps it is necessary because of the cross-domain/cross-thread nature of the api call? idfk. i see myself getting back into rpc generator land (shit i mentioned it... i know where this is heading now ffff), but damnit i want to hardcode the impl that rpc generator generates. i see and try to take many shortcuts to even that, but they tend to fuck everything (namely: me) up and i get lost in abstraction
}
void CleanRoomWidget::handleFrontPageDefaultViewReceived(QList<QString> frontPageDocs)
{
    //TODOreq: ez
}
#if 0
void CleanRoomWidget::respond(ICleanRoomFrontEndResponder *response)
{
    //we are on 'business' thread, which may not be the same thread as the 'gui' thread (but in this case/atm, it is xD)... so we still should propagate the response to the front-end in a thread/context (wt) safe manner from here
    SessionUserData sessionUserData = response->m_Request->m_Session->m_SessionUserData;
    WServer::post(sessionUserData.Ptr, sessionUserData.str0);

    response->respond(request);
}
#endif
#if 0
void CleanRoomWidget::respond(ICleanRoomFrontEndResponder *response)
{
    //we are on 'business' thread, which may not be the same thread as the 'gui' thread (but in this case/atm, it is xD)... so we still should propagate the response to the front-end in a thread/context (wt) safe manner from here
    SessionUserData sessionUserData = response->m_Request->m_Session->m_SessionUserData;
    WServer::post(sessionUserData.Ptr, sessionUserData.str0);
}
#endif
