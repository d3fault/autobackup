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
#define DocumentTimeline_QSettingsApplication_aka_DbName "DocumentTimeline"
#define DocumentTimelineDeclarationsOfIntentToAttemptRegistration_QSettingsApplication_aka_DbName DocumentTimeline_QSettingsApplication_aka_DbName "DeclarationsOfIntentToAttemptRegistration"
#define DocumentTimelineRegistrationAttempts_QSettingsApplication_aka_DbName DocumentTimeline_QSettingsApplication_aka_DbName "RegistrationAttempts"

DocumentTimeline::DocumentTimeline(QObject *parent)
    : IDocumentTimeline(parent)
    , m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimelineDeclarationsOfIntentToAttemptRegistration_QSettingsApplication_aka_DbName, this))
    , m_DocumentTimelineRegistrationAttemptsDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimelineRegistrationAttempts_QSettingsApplication_aka_DbName, this))
    , m_DocumentTimelineDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimeline_QSettingsApplication_aka_DbName, this))
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
QString DocumentTimeline::generateB64Salt(QString seed) //TODOmb: more cryptographically secure-er
{
    qint64 currentMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    QByteArray saltBuffer = QString::number(currentMSecsSinceEpoch).toUtf8();
    saltBuffer.append(seed.toUtf8());
    qsrand(currentMSecsSinceEpoch);
#define SEED_PRE_B64_MAX_LEN 16
    QByteArray saltPreB64 = QCryptographicHash::hash(saltBuffer, QCryptographicHash::Sha1);
    int saltPreB64LenActual = qMin(saltPreB64.length(), SEED_PRE_B64_MAX_LEN);
    int startIndex = qrand() % (saltPreB64.length()-saltPreB64LenActual);
    QByteArray saltSelectedRange = saltPreB64.mid(startIndex, saltPreB64LenActual);
    QByteArray saltB64 = saltSelectedRange.toBase64();
    QString ret(saltB64);
    return ret;
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
    if(m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->contains(desiredUsername)/* || m_DocumentTimelineRegistrationAttemptsDb->contains(desiredUsername)*/)
    {
        request->respond(false, QString());
        return;
    }
    QJsonObject registrationAttemptDeclarationJsonObject;
    registrationAttemptDeclarationJsonObject.insert("fullName", fullName);
    QString passwordSalt = generateB64Salt(fullName + desiredUsername + password);
    QString saltedPasswordHashB64 = salt(password, passwordSalt);
    registrationAttemptDeclarationJsonObject.insert("saltedPasswordHashB64", saltedPasswordHashB64);
    registrationAttemptDeclarationJsonObject.insert("passwordSaltB64", passwordSalt);
    registrationAttemptDeclarationJsonObject.insert("acceptedContributorLicenseAgreement", acceptedCLA);
    registrationAttemptDeclarationJsonObject.insert("fullNameSignature", fullNameSignature);
    QJsonDocument jsonDocument(registrationAttemptDeclarationJsonObject);
    QByteArray jsonByteArray = jsonDocument.toJson(QJsonDocument::Compact);
    QString jsonString(jsonByteArray);
    m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->setValue(desiredUsername, jsonString);

    //now that the intent to register has been declared, generate the string the user must recite in their registration attempt video. it is a b64sha1 of their 'declaration of attempt to register' json doc
    QByteArray sha1OfRegistrationAttemptDeclaration = QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Sha1);
    QByteArray b64sha1OfRegistrationDetailsKeyAndValuePersistedToDisk_ForUserToReciteOnVidByteArray = sha1OfRegistrationAttemptDeclaration.toBase64();
    QString b64sha1OfRegistrationDetailsKeyAndValuePersistedToDisk_ForUserToReciteOnVid(b64sha1OfRegistrationDetailsKeyAndValuePersistedToDisk_ForUserToReciteOnVidByteArray);

    request->respond(true, b64sha1OfRegistrationDetailsKeyAndValuePersistedToDisk_ForUserToReciteOnVid);
}
void DocumentTimeline::submitRegistrationAttemptVideo(IDocumentTimelineSubmitRegistrationAttemptVideoRequest *request, QString desiredUsername, QString password, QString registrationAttemptSubmissionVideoLocalFilePath)
{
    QString declarationOfRegistrationAttemptJsonStringMaybe = m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->value(desiredUsername).toString();
    if((declarationOfRegistrationAttemptJsonStringMaybe.isEmpty()) || (m_DocumentTimelineRegistrationAttemptsDb->contains(desiredUsername))) //registration attempt must already be declared, registration attempt must not have been recieved yet
    {
        request->respond(false);
        return;
    }

    QByteArray declarationOfRegistrationAttemptJsonByteArray = declarationOfRegistrationAttemptJsonStringMaybe.toUtf8();
    QJsonParseError parseError;
    QJsonDocument declarationOfRegistrationAttemptJson = QJsonDocument::fromJson(declarationOfRegistrationAttemptJsonByteArray, parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        request->respond(false); //TODOmb: OT: request could be a shared pointer (perhaps explicitly shared? idfk (implementation detail is implementation detail? really a typedef QSharedPointer<BlahRequest> Request; would do)). anyways, when the last reference goes out of scope, respond is called. 'success' is a property of every request, and it defaults to true. so anywhere you have an error case, you can merely 'return;'. If you want to continue asynchronously processing the request, you save it somewhere until you are ready to continue asynchronously processing it. the saving of it increases the reference count, which postpones the respond()'ing. the async 'continue' callback itself is the best place to store the copy of the request, becuase then when that method goes out of scope, the reference count is decremented like desired. semi-ot: a requirement of this is that every response arg type must be default constructable (i still like my "one api-request-response 'finished' handler per api-request in the front-end" design, because it funnels the codepaths down to one)
        return;
    }
    if(!declarationOfRegistrationAttemptJson.isObject())
    {
        request->respond(false);
        return;
    }
    QJsonObject declarationOfRegistrationAttemptJsonObject = declarationOfRegistrationAttemptJson.object();
    QByteArray registrationAttemptSubmissionVideoFileContents = filePathToQByteArray(registrationAttemptSubmissionVideoLocalFilePath);
    QString registrationAttemptSubmissionVideoFileContentsString(registrationAttemptSubmissionVideoFileContents.toBase64());
    declarationOfRegistrationAttemptJsonObject.insert("registrationAttemptSubmissionVideoB64", registrationAttemptSubmissionVideoFileContentsString);

    QString registrationAttemptJsonByteArray = QString(QJsonDocument(declarationOfRegistrationAttemptJsonObject).toJson(QJsonDocument::Compact));
    m_DocumentTimelineRegistrationAttemptsDb->setValue(desiredUsername, registrationAttemptJsonByteArray);
    //m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->remove(desiredUsername); <-- we don't/can't remove it because it opens up race conditions that I don't want to deal with

    request->respond(true);
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
