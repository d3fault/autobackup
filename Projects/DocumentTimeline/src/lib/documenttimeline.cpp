//##################
//This file is an Rpc Generated Business Implementation Skeleton file. Changing it is allowed. If you delete this file, it will be re-generated to it's default state the next time the Rpc Generator is run. If you really want to ignore it's contents, just put the entire file's contents in an 'if 0'
//##################

#include "documenttimeline.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMapIterator>
#include <QSettings>
#include <QCryptographicHash>

#include "documenttimelinecommon.h"

#define DocumentTimeline_ORGANIZATION "DocumentTimelineOrganization"
#define DocumentTimeline_APPLICATION "DocumentTimeline"
#define DocumentTimelineRegistrationAttempts_APPLICATION DocumentTimeline_APPLICATION "RegistrationAttempts"

DocumentTimeline::DocumentTimeline(QObject *parent)
    : IDocumentTimeline(parent)
    , m_DocumentTimelineRegistrationAttemptsDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimelineRegistrationAttempts_APPLICATION, this))
    , m_DocumentTimelineDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimeline_APPLICATION, this))
{
    QStringList docKeys = m_DocumentTimelineDb->childKeys();
    if(docKeys.isEmpty())
    {
        //TODOreq: this is debug test shit
        for(int i = 0; i < NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE; ++i)
        {
            QJsonObject newDebugTestDocJsonObject;
            newDebugTestDocJsonObject.insert("t", QString::number(QDateTime::currentMSecsSinceEpoch()));
            newDebugTestDocJsonObject.insert("u", "d3fault_" + QString::number(i));
            QString dataB64 = QString(QString("poopLoL_" + QString::number(i)).toUtf8().toBase64());
            newDebugTestDocJsonObject.insert("d", dataB64);
            newDebugTestDocJsonObject.insert("l", "DPL3+");
            QJsonDocument newDebugTestJsonDoc = QJsonDocument(newDebugTestDocJsonObject);
            QByteArray newDebugTestDoc = newDebugTestJsonDoc.toJson(QJsonDocument::Compact);
            m_DocumentTimelineDb->setValue(documentJsonToHexHash(newDebugTestDoc), newDebugTestDoc);
        }
    }
}
QByteArray DocumentTimeline::documentJsonToHexHash(const QByteArray &documentJson)
{
    return QCryptographicHash::hash(documentJson, QCryptographicHash::Sha1).toHex();
}
void DocumentTimeline::getLatestDocuments(IDocumentTimelineGetLatestDocumentsRequest *request)
{
    //TODOoptimization: this is an inefficient, but functional, QSettings-based implementation. Ideally, it would be a couchbase view query or something
    QStringList docKeys = m_DocumentTimelineDb->childKeys();
    QMultiMap<QDateTime, QByteArray> docSorter;
    Q_FOREACH(QString docKey, docKeys)
    {
        QByteArray doc = m_DocumentTimelineDb->value(docKey).toByteArray();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(doc);
        QJsonObject jsonDocObject = jsonDoc.object();
        QJsonValue timestampMaybe = jsonDocObject.value("t");
        Q_ASSERT(timestampMaybe.type() != QJsonValue::Undefined); //a non-tudl doc got in!
        QDateTime docTimestamp = QDateTime::fromMSecsSinceEpoch(timestampMaybe.toString().toLongLong());
        docSorter.insert(docTimestamp, doc);
    }
    QList<QByteArray> ret;
    QMapIterator<QDateTime, QByteArray> sortedDocsIterator(docSorter);
    while(sortedDocsIterator.hasNext())
    {
        sortedDocsIterator.next();
        ret.append(sortedDocsIterator.value());
        if(ret.size() == NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE)
            break;
    }
    //return ret;
    request->respond(true, ret);
}
void DocumentTimeline::declareIntentToAttemptRegistration(IDocumentTimelineDeclareIntentToAttemptRegistrationRequest *request, QString fullName, QString desiredUsername, QString password, bool acceptedCLA, QString fullNameSignature)
{
    if(m_DocumentTimelineRegistrationAttemptsDb->contains(desiredUsername))
    {
        request->respond(false, QString());
        return;
    }
    QJsonObject registrationAttemptDeclarationJsonObject;
    registrationAttemptDeclarationJsonObject.insert("fullName", fullName);
    registrationAttemptDeclarationJsonObject.insert("passwordHashB64", password);
    registrationAttemptDeclarationJsonObject.insert("passwordSaltB64", "TODOreq: blah");
    registrationAttemptDeclarationJsonObject.insert("acceptedContributorLicenseAgreement", acceptedCLA);
    registrationAttemptDeclarationJsonObject.insert("fullNameSignature", fullNameSignature);
    QJsonDocument jsonDocument(registrationAttemptDeclarationJsonObject);
    QString json(jsonDocument.toJson(QJsonDocument::Compact));
    m_DocumentTimelineRegistrationAttemptsDb->setValue(desiredUsername, json);
    request->respond(true, "TODOreq: some hash of their registration attempt details (and some crypto randomness probably) or something");
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
