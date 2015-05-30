//##################
//This file is an Rpc Generated Business Implementation Skeleton file. Changing it is allowed. If you delete this file, it will be re-generated to it's default state the next time the Rpc Generator is run. If you really want to ignore it's contents, just put the entire file's contents in an 'if 0'
//##################

#ifndef DOCUMENTTIMELINE_H
#define DOCUMENTTIMELINE_H

#include "idocumenttimeline.h"

#include <QFile>
#include <QCryptographicHash>

class QSettings;

class DocumentTimeline : public IDocumentTimeline
{
    Q_OBJECT
public:
    explicit DocumentTimeline(QObject *parent = 0);
private:
    QSettings *m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb;
    QSettings *m_DocumentTimelineRegistrationAttemptsDb;
    QSettings *m_DocumentTimelineRegistrationAttemptsApprovalsOrRejectionsDb;
    QSettings *m_DocumentTimelineRegisteredUsersDb;
    QSettings *m_DocumentTimelineDb;

    static QByteArray documentJsonToHexHash(const QByteArray &documentJson);
    static QString generateB64Salt(QString seed);
    static inline QByteArray filePathToQByteArray(QString filePath)
    {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly))
            return QByteArray();
        return file.readAll();
    }
    static inline QString b64hashSaltedPassword(QString password, QString passwordSalt)
    {
        QString passwordSalted = password + passwordSalt; //woot
        QByteArray passwordSaltedAsByteArrayForHashing = passwordSalted.toUtf8();
        QByteArray passwordSaltedSha1 = QCryptographicHash::hash(passwordSaltedAsByteArrayForHashing, QCryptographicHash::Sha1);
        QString ret(passwordSaltedSha1.toBase64());
        return ret;
    }
public slots:
    void getLatestDocuments(IDocumentTimelineGetLatestDocumentsRequest *request);
    void declareIntentToAttemptRegistration(IDocumentTimelineDeclareIntentToAttemptRegistrationRequest *request, QString fullName, QString desiredUsername, QString password, bool acceptedCLA, QString fullNameSignature);
    void submitRegistrationAttemptVideo(IDocumentTimelineSubmitRegistrationAttemptVideoRequest *request, QString desiredUsername, QString password, QString registrationAttemptSubmissionVideoLocalFilePath);
    void login(IDocumentTimelineLoginRequest *request, QString username, QString password);
    void post(IDocumentTimelinePostRequest *request, QString username, QByteArray data, QString licenseIdentifier);
    void registrationVideoApprover_getOldestNotDoneRegistrationAttempsVideo(IDocumentTimelineRegistrationVideoApprover_getOldestNotDoneRegistrationAttempsVideoRequest *request);
    void registrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideo(IDocumentTimelineRegistrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideoRequest *request, bool acceptIfTrue_rejectIfFalse, QString usernameAttemptingToRegister);
    void logout(IDocumentTimelineLogoutRequest *request);
};

#endif // DOCUMENTTIMELINE_H
