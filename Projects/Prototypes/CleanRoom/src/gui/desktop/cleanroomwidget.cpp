#include "cleanroomwidget.h"

CleanRoomWidget::CleanRoomWidget(QWidget *parent)
    : QWidget(parent)
    , ICleanRoomSession(m_CleanRoom, new CleanRoomGuiSessionResponder(this))
{ }
void CleanRoomWidget::handleRegistrationAttemptFinished(ICleanRoomSessionRequest *request)
{
    RegisterSessionRequest *requestAsActual = static_cast<RegisterSessionRequest*>(request);
    setWindowTitle(tr("Registration attempt: " + (requestAsActual->response()->success() ? "success" : "fail")));
}
void CleanRoomWidget::handleRegisterButtonClicked()
{
    QString username, password; //TODOreq
    /*emit */sessionRequestRequested(createRegisterSessionRequest(username, password, &CleanRoomWidget::handleRegistrationAttemptFinished));
}
void CleanRoomWidget::handleLoginButtonClicked()
{

}
void CleanRoomWidget::handleSubmitButtonClicked()
{
    QByteArray data; //TODOreq
    /*emit */sessionRequestRequested(createSubmitDataSessionRequest(data));
}
void CleanRoomWidget::handleLogoutButtonClicked()
{

}
#if 0
void CleanRoomWidget::respond(ICleanRoomSessionResponder *response)
{
    //we are on 'business' thread, which may not be the same thread as the 'gui' thread (but in this case/atm, it is xD)... so we still should propagate the response to the front-end in a thread/context (wt) safe manner from here
    SessionUserData sessionUserData = response->m_Request->m_Session->m_SessionUserData;
    WServer::post(sessionUserData.Ptr, sessionUserData.str0);
}
#endif
