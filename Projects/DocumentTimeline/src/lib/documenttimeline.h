//##################
//This file is an Rpc Generated Business Implementation Skeleton file. Changing it is allowed. If you delete this file, it will be re-generated to it's default state the next time the Rpc Generator is run. If you really want to ignore it's contents, just put the entire file's contents in an 'if 0'
//##################

#ifndef CLEANROOM_H
#define CLEANROOM_H

#include "idocumenttimeline.h"

class QSettings;

class DocumentTimeline : public IDocumentTimeline
{
    Q_OBJECT
public:
    explicit DocumentTimeline(QObject *parent = 0);
private:
    QSettings *m_DocumentTimelineRegistrationAttemptsDb;
    QSettings *m_DocumentTimelineDb;

    static QByteArray documentJsonToHexHash(const QByteArray &documentJson);
public slots:
    void getLatestDocuments(IDocumentTimelineGetLatestDocumentsRequest *request);
    void declareIntentToAttemptRegistration(IDocumentTimelineDeclareIntentToAttemptRegistrationRequest *request, QString fullName, QString desiredUsername, QString password, bool acceptedCLA, QString fullNameSignature);
    void submitRegistrationAttemptVideo(IDocumentTimelineSubmitRegistrationAttemptVideoRequest *request, QString desiredUsername, QString password, QByteArray registrationAttemptSubmissionVideo);
    void login(IDocumentTimelineLoginRequest *request, QString username, QString password);
    void post(IDocumentTimelinePostRequest *request, QString username, QByteArray data, QString licenseIdentifier);
    void registrationVideoApprover_getOldestNotDoneRegistrationAttempsVideo(IDocumentTimelineRegistrationVideoApprover_getOldestNotDoneRegistrationAttempsVideoRequest *request);
    void registrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideo(IDocumentTimelineRegistrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideoRequest *request, bool acceptIfTrue_rejectIfFalse, QString username);
    void logout(IDocumentTimelineLogoutRequest *request);
};

#endif // CLEANROOM_H
