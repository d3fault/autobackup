//##################
//This file is an Rpc Generated Business Implementation Skeleton file. Changing it is allowed. If you delete this file, it will be re-generated to it's default state the next time the Rpc Generator is run. If you really want to ignore it's contents, just put the entire file's contents in an 'if 0'
//##################

#include "documenttimeline.h"

DocumentTimeline::DocumentTimeline(QObject *parent)
    : IDocumentTimeline(parent)
{ }
void DocumentTimeline::getLatestDocuments(IDocumentTimelineGetLatestDocumentsRequest *request)
{
    qFatal("DocumentTimeline::getLatestDocuments(IDocumentTimelineGetLatestDocumentsRequest *request) not yet implemented");
}
void DocumentTimeline::declareIntentToAttemptRegistration(IDocumentTimelineDeclareIntentToAttemptRegistrationRequest *request, QString fullName, QString desiredUsername, QString password, bool acceptedCLA, QString fullNameSignature)
{
    qFatal("DocumentTimeline::declareIntentToAttemptRegistration(IDocumentTimelineDeclareIntentToAttemptRegistrationRequest *request, QString fullName, QString desiredUsername, QString password, bool acceptedCLA, QString fullNameSignature) not yet implemented");
}
void DocumentTimeline::submitRegistrationAttemptVideo(IDocumentTimelineSubmitRegistrationAttemptVideoRequest *request, QString desiredUsername, QString password, QByteArray registrationAttemptSubmissionVideo)
{
    qFatal("DocumentTimeline::submitRegistrationAttemptVideo(IDocumentTimelineSubmitRegistrationAttemptVideoRequest *request, QString desiredUsername, QString password, QByteArray registrationAttemptSubmissionVideo) not yet implemented");
}
void DocumentTimeline::login(IDocumentTimelineLoginRequest *request, QString username, QString password)
{
    qFatal("DocumentTimeline::login(IDocumentTimelineLoginRequest *request, QString username, QString password) not yet implemented");
}
void DocumentTimeline::post(IDocumentTimelinePostRequest *request, QString username, QByteArray data, QString licenseIdentifier)
{
    qFatal("DocumentTimeline::post(IDocumentTimelinePostRequest *request, QString username, QByteArray data, QString licenseIdentifier) not yet implemented");
}
void DocumentTimeline::registrationVideoApprover_getOldestNotDoneRegistrationAttempsVideo(IDocumentTimelineRegistrationVideoApprover_getOldestNotDoneRegistrationAttempsVideoRequest *request)
{
    qFatal("DocumentTimeline::registrationVideoApprover_getOldestNotDoneRegistrationAttempsVideo(IDocumentTimelineRegistrationVideoApprover_getOldestNotDoneRegistrationAttempsVideoRequest *request) not yet implemented");
}
void DocumentTimeline::registrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideo(IDocumentTimelineRegistrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideoRequest *request, bool acceptIfTrue_rejectIfFalse, QString username)
{
    qFatal("DocumentTimeline::registrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideo(IDocumentTimelineRegistrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideoRequest *request, bool acceptIfTrue_rejectIfFalse, QString username) not yet implemented");
}
void DocumentTimeline::logout(IDocumentTimelineLogoutRequest *request)
{
    qFatal("DocumentTimeline::logout(IDocumentTimelineLogoutRequest *request) not yet implemented");
}
