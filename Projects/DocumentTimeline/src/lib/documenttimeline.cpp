//##################
//This file is an Rpc Generated Business Implementation Skeleton file. Changing it is allowed. If you delete this file, it will be re-generated to it's default state the next time the Rpc Generator is run. If you really want to ignore it's contents, just put the entire file's contents in an 'if 0'
//##################

#include "documenttimeline.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMapIterator>
#include <QSettings>
#include <QCryptographicHash>
#include <QDateTime>

#include "documenttimelinecommon.h"
#include "documenttimelinedb.h"

#if 0
#define DocumentTimelineDeclarationsOfIntentToAttemptRegistration_QSettingsApplication_aka_DbName DocumentTimeline_QSettingsApplication_aka_DbName "DeclarationsOfIntentToAttemptRegistration"
#define DocumentTimelineRegistrationAttempts_QSettingsApplication_aka_DbName DocumentTimeline_QSettingsApplication_aka_DbName "RegistrationAttempts"
#define DocumentTimelineRegistrationAttemptsApprovalsOrRejections_QSettingsApplication_aka_DbName DocumentTimelineRegistrationAttempts_QSettingsApplication_aka_DbName "ApprovalsOrRejections"
#define DocumentTimelineRegisteredUsers__QSettingsApplication_aka_DbName DocumentTimeline_QSettingsApplication_aka_DbName "RegisteredUsers"
#endif

//TODOreq: redo the API so that registration attempts (both declarations of attempts, and the attempt videos themselves) are posted on the DocumentTimeline by some special username "DocumentTimelineRegistrationAttempts" (or something). This means that the registration attempt must itself must be >= DPL, of course
//OT'ish: I could make my QSettings-asap-usage mimic couchbase 1:1, where I store the data by key and the key is b64sha1(data).... BUT then the QSettings impl would be SLOW AS BALLS. Better to choose an ad-hoc/temporary 'primary key' for storage/retrieval (desiredUsername, in the case of registration attempts)
//TODOprobably: 'registration video attempt' approver tiers. I am at the top tier, can approve any registration video attempt by myself/etc (admin mode or whatever). A more scalable strategy is to have every user be a 'registration video attempt' approver, but to require some N number of approvals, and some M number of approvals by a "moderator"-tier approver. So just a random example: registration requires 10x approvals from peers, and 1x approval from moderator. I can of course tier it up to infinite precision (HEH, SOUNDS/LOOKS LIKE "tear it up")). For now and to KISS, 1x admin-tier user (me).
//Note: how the qsettings-asep db impl stores the data is not too important. what IS is important is that the interactions between the business and the db (DocumentTimelineDb) are compatible-with and tuned-to couchbase db interactions (regular get/post shit, or view query)
//TODOmb: the 'session' should be implicitly shared. when logging in, the front-end sends the session as one of the api call args (along with username/pw). the business checks the username/pw, changes the 'session' by calling setLogin(true) (note: this detaches from the copy the front-end is storing), and then returns the 'session' as an api call response arg... where the front-end overwrites (detaches/probably-destroys the last copy of the old/NOT-logged-in session) it's byval copy of 'session' using simple assignment. In order for this to work, none of the 'request-new-api-call-X' methods on the session can be non-const (I don't think this is an issue, but I might be wrong). --- IDEALLY in a future version of RpcGenerator, an application's "auth" stuff could/would be built in and transparent (the same login procedure would be used, but the user wouldn't have to create the "login" api call (with the session as request/response args)... they'd simply say "the application uses basic authentication" to the rpc generator or some such)
DocumentTimeline::DocumentTimeline(QObject *parent)
    : IDocumentTimeline(parent)
    , m_Db(new DocumentTimelineDb(this))
    //, m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimelineDeclarationsOfIntentToAttemptRegistration_QSettingsApplication_aka_DbName, this))
    //, m_DocumentTimelineRegistrationAttemptsDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimelineRegistrationAttempts_QSettingsApplication_aka_DbName, this))
    //, m_DocumentTimelineRegistrationAttemptsApprovalsOrRejectionsDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimelineRegistrationAttemptsApprovalsOrRejections_QSettingsApplication_aka_DbName, this))
    //, m_DocumentTimelineRegisteredUsersDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, , this))
    //, m_DocumentTimelineDb(new QSettings(QSettings::IniFormat, QSettings::UserScope, DocumentTimeline_ORGANIZATION, DocumentTimeline_QSettingsApplication_aka_DbName, this))
{
//    QStringList docKeys = m_DocumentTimelineDb->childKeys();
//    if(docKeys.isEmpty())
//    {
//        //TODOreq: this is debug test shit
//        for(int i = 0; i < NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE; ++i)
//        {
//            QJsonObject newDebugTestDocJsonObject;
//            newDebugTestDocJsonObject.insert("t", QString::number(QDateTime::currentMSecsSinceEpoch()));
//            newDebugTestDocJsonObject.insert("u", "d3fault_" + QString::number(i));
//            QString dataB64 = QString(QString("poopLoL_" + QString::number(i)).toUtf8().toBase64());
//            newDebugTestDocJsonObject.insert("d", dataB64);
//            newDebugTestDocJsonObject.insert("l", "DPL3+");
//            QJsonDocument newDebugTestJsonDoc = QJsonDocument(newDebugTestDocJsonObject);
//            QByteArray newDebugTestDoc = newDebugTestJsonDoc.toJson(QJsonDocument::Compact);
//            m_DocumentTimelineDb->setValue(documentJsonToHexHash(newDebugTestDoc), newDebugTestDoc);
//        }
//    }

    connect(m_Db, &DocumentTimelineDb::addDocToDbFinished, this, &DocumentTimeline::handleAddToDbFinished);
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
void DocumentTimeline::getLatestDocuments(IDocumentTimelineGetLatestDocumentsRequest *request) //becomes couchbase-json-view-query
{
//    //TODOoptimization: this is an inefficient, but functional, QSettings-based implementation. Ideally, it would be a couchbase view query or something
//    QStringList docKeys = m_DocumentTimelineDb->childKeys();
//    QMultiMap<QDateTime, QByteArray> docSorter;
//    Q_FOREACH(QString docKey, docKeys)
//    {
//        QByteArray doc = m_DocumentTimelineDb->value(docKey).toByteArray();
//        QJsonDocument jsonDoc = QJsonDocument::fromJson(doc);
//        QJsonObject jsonDocObject = jsonDoc.object();
//        QJsonValue timestampMaybe = jsonDocObject.value("t");
//        Q_ASSERT(timestampMaybe.type() != QJsonValue::Undefined); //a non-tudl doc got in!
//        QDateTime docTimestamp = QDateTime::fromMSecsSinceEpoch(timestampMaybe.toString().toLongLong());
//        docSorter.insert(docTimestamp, doc);
//    }
//    QList<QByteArray> ret;
//    QMapIterator<QDateTime, QByteArray> sortedDocsIterator(docSorter);
//    while(sortedDocsIterator.hasNext())
//    {
//        sortedDocsIterator.next();
//        ret.append(sortedDocsIterator.value());
//        if(ret.size() == NUM_DOCUMENT_TIMELINE_DOCS_ON_FRONT_PAGE)
//            break;
//    }
//    //return ret;
//    request->respond(true, ret);
}
void DocumentTimeline::declareIntentToAttemptRegistration(IDocumentTimelineDeclareIntentToAttemptRegistrationRequest *request, QString fullName, QString desiredUsername, QString password, bool acceptedCLA, QString fullNameSignature)
{
//    if(m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->contains(desiredUsername)/* || m_DocumentTimelineRegistrationAttemptsDb->contains(desiredUsername)*/)
//    {
//        request->respond(false, QString());
//        return;
//    }

#if 0
    QJsonObject registrationAttemptDeclarationJsonObject;
    registrationAttemptDeclarationJsonObject.insert("preDocumentTimelineRegistrationDocument", true);
    registrationAttemptDeclarationJsonObject.insert("desiredUsername", desiredUsername);
    registrationAttemptDeclarationJsonObject.insert("fullName", fullName);
    QString passwordSalt = generateB64Salt(fullName + desiredUsername + password);
    QString saltedPasswordHashB64 = b64hashSaltedPassword(password, passwordSalt);
    registrationAttemptDeclarationJsonObject.insert("saltedPasswordHashB64", saltedPasswordHashB64);
    registrationAttemptDeclarationJsonObject.insert("passwordSaltB64", passwordSalt);
    registrationAttemptDeclarationJsonObject.insert("acceptedContributorLicenseAgreement", acceptedCLA);
    registrationAttemptDeclarationJsonObject.insert("fullNameSignature", fullNameSignature);

    //now that the intent to register has been created, generate the string the user must recite in their registration attempt video. it is a b64sha1 of their 'declaration of attempt to register' json doc, which incidentally also goes in the declaration of attempt to register (so we can't calculate it again without first removing it again (but don't need to calculate it again so fuck it))
    QJsonDocument jsonDocumentBeforeStringForUserToReciteAdded(registrationAttemptDeclarationJsonObject);
    QByteArray jsonByteArrayBeforeStringForUserToReciteAdded = jsonDocumentBeforeStringForUserToReciteAdded.toJson(QJsonDocument::Compact);
    QByteArray sha1OfRegistrationAttemptDeclaration = QCryptographicHash::hash(jsonByteArrayBeforeStringForUserToReciteAdded, QCryptographicHash::Sha1);
    QByteArray b64sha1OfRegistrationDetails_ForUserToReciteOnVidByteArray = sha1OfRegistrationAttemptDeclaration.toBase64();
#define DOCUMENT_TIMELINE_B64_SHA1_OF_REGISTRATION_DETAILS_FOR_USER_TO_RECITE_ON_VID "b64sha1OfRegistrationDetails_ForUserToReciteOnVid"
    QString b64sha1OfRegistrationDetails_ForUserToReciteOnVid(b64sha1OfRegistrationDetails_ForUserToReciteOnVidByteArray);
    registrationAttemptDeclarationJsonObject.insert(DOCUMENT_TIMELINE_B64_SHA1_OF_REGISTRATION_DETAILS_FOR_USER_TO_RECITE_ON_VID, b64sha1OfRegistrationDetails_ForUserToReciteOnVid);

    QByteArray jsonByteArray = QJsonDocument(registrationAttemptDeclarationJsonObject).toJson(QJsonDocument::Compact);
    QString jsonString(jsonByteArray);
    m_Db->addDocToDb(jsonString);
#endif
}
void DocumentTimeline::submitRegistrationAttemptVideo(IDocumentTimelineSubmitRegistrationAttemptVideoRequest *request, QString desiredUsername, QString password, QString registrationAttemptSubmissionVideoLocalFilePath)
{
//    QString declarationOfRegistrationAttemptJsonStringMaybe = m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->value(desiredUsername).toString();
//    if((declarationOfRegistrationAttemptJsonStringMaybe.isEmpty()) || (m_DocumentTimelineRegistrationAttemptsDb->contains(desiredUsername))) //registration attempt must already be declared, registration attempt must not have been recieved yet
//    {
//        request->respond(false);
//        return;
//    }

//    QByteArray declarationOfRegistrationAttemptJsonByteArray = declarationOfRegistrationAttemptJsonStringMaybe.toUtf8();
//    QJsonParseError parseError;
//    QJsonDocument declarationOfRegistrationAttemptJson = QJsonDocument::fromJson(declarationOfRegistrationAttemptJsonByteArray, &parseError);
//    if(parseError.error != QJsonParseError::NoError)
//    {
//        request->respond(false); //TODOmb: OT: request could be a shared pointer (perhaps explicitly shared? idfk (implementation detail is implementation detail? really a typedef QSharedPointer<BlahRequest> Request; would do)). anyways, when the last reference goes out of scope, respond is called. 'success' is a property of every request, and it defaults to true. so anywhere you have an error case, you can merely 'return;'. If you want to continue asynchronously processing the request, you save it somewhere until you are ready to continue asynchronously processing it. the saving of it increases the reference count, which postpones the respond()'ing. the async 'continue' callback itself is the best place to store the copy of the request, becuase then when that method goes out of scope, the reference count is decremented like desired. semi-ot: a requirement of this is that every response arg type must be default constructable (i still like my "one api-request-response 'finished' handler per api-request in the front-end" design, because it funnels the codepaths down to one)
//        return;
//    }
//    if(!declarationOfRegistrationAttemptJson.isObject())
//    {
//        request->respond(false);
//        return;
//    }
//    QJsonObject declarationOfRegistrationAttemptJsonObject = declarationOfRegistrationAttemptJson.object();
//    QByteArray registrationAttemptSubmissionVideoFileContents = filePathToQByteArray(registrationAttemptSubmissionVideoLocalFilePath);
//    QString registrationAttemptSubmissionVideoFileContentsString(registrationAttemptSubmissionVideoFileContents.toBase64());
//#define DOCUMENT_TIMELINE_REGISTRATION_ATTEMPT_SUBMISSION_VIDEO_B64_JSONKEY "registrationAttemptSubmissionVideoB64"
//    declarationOfRegistrationAttemptJsonObject.insert(DOCUMENT_TIMELINE_REGISTRATION_ATTEMPT_SUBMISSION_VIDEO_B64_JSONKEY, registrationAttemptSubmissionVideoFileContentsString);
//#define DOCUMENT_TIMELINE_REGISTRATION_ATTEMPT_SUBMISSION_VIDEO_SUBMITTED_AT_TIMESTAMP_JSONKEY "registrationAttemptSubmissionVideoSubmittedAtTimestamp"
//    declarationOfRegistrationAttemptJsonObject.insert(DOCUMENT_TIMELINE_REGISTRATION_ATTEMPT_SUBMISSION_VIDEO_SUBMITTED_AT_TIMESTAMP_JSONKEY, QString::number(QDateTime::currentMSecsSinceEpoch()));

//    QString registrationAttemptJsonByteArray = QString(QJsonDocument(declarationOfRegistrationAttemptJsonObject).toJson(QJsonDocument::Compact));
//    m_DocumentTimelineRegistrationAttemptsDb->setValue(desiredUsername, registrationAttemptJsonByteArray);
//    //m_DocumentTimelineDocumentTimelineDeclarationsOfIntentToAttemptRegistrationDb->remove(desiredUsername); <-- we don't/can't remove it because it opens up race conditions that I don't want to deal with

//    request->respond(true);
}
void DocumentTimeline::login(IDocumentTimelineLoginRequest *request, QString username, QString password)
{
//    //TODOreq: if the username/pw isn't found in the registered users db, we can/should check (a la inline recovery) m_DocumentTimelineRegistrationAttemptsApprovalsOrRejectionsDb for the username. If it's in there (and 'accepted' == true), it means that the system failed right after putting it in there but before adding the username/pw to the registered users db. So we can do that now for inline recovery. If it's not in there, well then it isn't a registered (maybe registration attempt still pending) user so we just request.respond(false). also should be noted that the 'first post on document timeline' (the announcemeent of the new user) can/should also be a part of that inline recovery
//    QString userDetailsJsonString = m_DocumentTimelineRegisteredUsersDb->value(username).toString();
//    if(!userDetailsJsonString)
//    {
//        request->respond(false);
//        return;
//    }

    //how the fuck would logging in work if 'every piece of data is a post'? I _CAN_ recreate the json doc used to calculate the sha1 key 1:1, ex-fucking-actomundo (if doc not in db, pw is wrong or user doesn't exist etc)......... EXCEPT for the 't' element onf 'tudl'. Fuck fuck what the fuck. Oh wait just realized the passwordSalt has the same problem. So 't' and 'passwordSalt' shit all over this 'every data item in app is a tudl post' design. wat do nao? Another maybe OT thing was that the addDocToDbFinished signal is only listened to in one slot, so we can only static_cast the userData back to IDocumentTimelinePostRequest..... which is fine when it's just a regular post: BUT.. there's a declare-intent-to-attempt-to-register 'post', submit-registration-attempt-video 'post, etc... that would still follow that same 'post' code path. Posting because a sub-request of those two 'register' api calls
}
void DocumentTimeline::post(IDocumentTimelinePostRequest *request, /*QString username, */QByteArray data, QString licenseIdentifier)
{
    if(!request->parentSession().loggedIn())
    {
        request->respond(false, false); //TODOmb: error strings? in this case, it'd simply be "login first" (obviously the front-end can/should check isLoggedIn before even us getting here (they both should do the check)). I think back in my old Rpc Generator 2 days I eventually landed on a FailureType enum, idk
        return;
    }
    QJsonObject postJsonObject;
    //timestamp 't' assigned by db -- TODOreq: once couchbase is integrated: whenevver a db submit has to 'retry' (exponential backoff), the corresponding timestamp should be updated (which, yes, changes the key LoL!)
    postJsonObject.insert("u", request->parentSession().loggedInUsername());
    postJsonObject.insert("d", QString(data.toBase64()));
    postJsonObject.insert("l", licenseIdentifier);
    m_Db->addDocToDb(QJsonDocument(postJsonObject).toJson(QJsonDocument::Compact), request);
}
void DocumentTimeline::registrationVideoApprover_getOldestNotDoneRegistrationAttempsVideo(IDocumentTimelineRegistrationVideoApprover_getOldestNotDoneRegistrationAttempsVideoRequest *request) //becomes couchbase-json-view-query
{
//    //TODOreq: the non-existence of some json flag (key name tbd) si what should indicate that a registration attempt video is 'not done' (aka, not yet processed by a video approver)
//    QMultiMap<QDateTime, QPair<QString /*desiredUsername*/, QByteArray /*registration attempt doc*/> > docSorter; //TODOoptimization: maybe store 'data key' instead of 'data value' as the multimap value? there's also another use of multimap in this class/file that could probably use the same optimization. for now, I'm sticking to the easiest-to-use API because 'optimization' means using couchbase!!! HOWEVER, the implicit sharing nature of QByteArray means that it is AS EXPENSIVE AS a QString-key in terms of performance (the data byte arrays are loaded into memory when the json is parsed no matter what) (excluding the fact that more free ram does have an impact on performance xD), BUT MORE EXPENSIVE MEMORY-WISE (since the data byte arrays are all in memory simultaneously xD)
//    Q_FOREACH(QString currentRegistrationAttemptDesiredUsername, m_DocumentTimelineRegistrationAttemptsDb->childKeys())
//    {
//        QJsonParseError parseError;
//        QByteArray registrationAttemptJsonByteArray = m_DocumentTimelineRegistrationAttemptsDb->value(currentRegistrationAttemptDesiredUsername).toString().toUtf8();
//        QJsonDocument registrationAttemptJson = QJsonDocument::fromJson(registrationAttemptJsonByteArray, &parseError);
//        if((parseError.error != QJsonParseError::NoError) || (!registrationAttemptJson.isObject()))
//        {
//            request->respond(false, QString(), QByteArray(), QString());
//            return;
//        }
//        QJsonObject registrationAttemptJsonObject = registrationAttemptJson.object();
//        if(registrationAttemptJsonObject.contains("registrationAttemptProcessedAs"))
//            continue; //already processed, aka 'done'
//        QPair<QString, QByteArray> docSorterEntry;
//        docSorterEntry.first = currentRegistrationAttemptDesiredUsername;
//        docSorterEntry.second = registrationAttemptJsonByteArray;
//        docSorter.insert(QDateTime::fromMSecsSinceEpoch(registrationAttemptJsonObject.value(DOCUMENT_TIMELINE_REGISTRATION_ATTEMPT_SUBMISSION_VIDEO_SUBMITTED_AT_TIMESTAMP_JSONKEY).toString().toLongLong()), docSorterEntry); //TODOoptional: check the parsing of the unix timestamp? I seem to be running into this question a lot: do I want to trust the contents of the db (meaning: do I want to trust my input sanitization?)... or should I sanitize the db contents when retrieving as well? A tough question to answer GENERALLY (it is perhaps use case specific)
//    }
//    /*QMapIterator<QDateTime, QByteArray> it(docSorter);
//    while(it.hasNext()) //TODOmb: maybe iterate in reverse, since we want 'oldest'? I can never remember how a QDateTime gets sorted :-P (and have Hvbs to blame)
//    {

//    }*/
//    if(docSorter.isEmpty())
//    {
//        request->respond(false, QString(), QByteArray(), QString()); //TODOreq: here's an example of me needing an lcbOpSuccess-esq bool. lcb op success doesn't fit EXACTLY, since I'm [going to be] a couchbase view query... but still, I need some way of differentiating between "dbError" (which translates into 'internalError' at this level of design?) and "foundAnotDoneRegistrationAttempt". TODOreq: I'm thinking now that maybe every api call should implicitly get a "bool internalError" response arg. "Internal Error" is better than DbError because it encapsulates it and covers more ground <-- way OT now: I think that if I ever implement the "reference counted" requests (where the request is responded to when the refcount becomes zero), the default value of the internalError should be false. Maybe every apiCall should be supplemented with an %apiCallSlotName%Success boolean, which defaults to false. So if you just "return;" that indicates that there wasn't an internal error but that the api call failed. Internal Errors must be detected/noticed, but Api Calls fail by default. There's also a world to explore within "response args types+varnames for when api call fails", and even different kinds of api call failures (with different kinds of arg types+varnames!), and of course a "error debug string" (specific error to stderr) and a "error user string" ('500 internal server error' presented to user)
//        return;
//    }

//    QPair<QString, QByteArray> oldestRegistrationAttempt_UsernameAndRegistrationAttemptJsonByteArray = docSorter.first(); //TODOmb: mabye last() instead, since we want 'oldest'? I can never remember how a QDateTime gets sorted :-P (and have Hvbs to blame)
//    QString oldestRegistrationAttempt_UsernameAttemptingToRegister = oldestRegistrationAttempt_UsernameAndRegistrationAttemptJsonByteArray.first;
//    QByteArray oldestRegistrationAttempt = oldestRegistrationAttempt_UsernameAndRegistrationAttemptJsonByteArray.second;
//    //already checked this once in this method:  QJsonParseError parseError;
//    QJsonDocument oldestRegistrationAttemptJson = QJsonDocument::fromJson(oldestRegistrationAttempt/*, &parseError*/); //TODOoptimization: maybe leave as QJsonObject (or document?) so I don't have to re-parse? I'm slowly mastering my usage of json, seeing all the different use cases of it before making a decision
//    /*if((parseError != QJsonParseError::NoError) || (!oldestRegistrationAttemptJson.isObject()))
//    {
//        request->respond(false, QString(), QByteArray(), QString());
//        return;
//    }*/
//    QJsonObject oldestRegistrationAttemptJsonObject = oldestRegistrationAttemptJson.object();
//    QByteArray registrationAttemptVideoByteArray = QByteArray::fromBase64(oldestRegistrationAttemptJsonObject.value(DOCUMENT_TIMELINE_REGISTRATION_ATTEMPT_SUBMISSION_VIDEO_B64_JSONKEY).toString().toUtf8());
//    QString dataUserMustHaveRecitedInRegistrationAttemptVideo = oldestRegistrationAttemptJsonObject.value(DOCUMENT_TIMELINE_B64_SHA1_OF_REGISTRATION_DETAILS_FOR_USER_TO_RECITE_ON_VID).toString();
//    request->respond(true, oldestRegistrationAttempt_UsernameAttemptingToRegister, registrationAttemptVideoByteArray, dataUserMustHaveRecitedInRegistrationAttemptVideo);
}
void DocumentTimeline::registrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideo(IDocumentTimelineRegistrationVideoAttemptApprover_acceptOrRejectRegistrationAttemptVideoRequest *request, bool acceptIfTrue_rejectIfFalse, QString usernameAttemptingToRegister)
{
//    //TODOreq(decided towards end, but still unsure of decision): my adding the json flag 'done' or 'processed' or whatever to the registration attempt marks my first 'mutation'. Should it be a separate doc/db/couchbase-bucket(which could also just be a couchbase-key-prefix)??? Or should I use LCB_SET + cas? Basically I'm seeing the whole 'journaling' design consideration again. I know for sure I don't want LCB_SET + cas in the timeline document db, but maybe it's ok in the registration attempt db? Still, it wouldn't be very hard to make it a separate doc so I could use LCB_ADD :-D... and maybe that's all there is to it to making a journal system to begin with? Maybe it's not as complicated as I think, just need to know when to make a separate doc (category) vs modifying an existing doc? SINCE it's not very difficult to make a separate doc (category), and since it means I get to only use LCB_ADD, I will.

//    if(m_DocumentTimelineRegistrationAttemptsApprovalsOrRejectionsDb->contains(usernameAttemptingToRegister)) //aka: LCB_ADD fails. might happen if there are multiple "video approvers" (care should be taken that it doesn't, in order to be more efficient with the video approvers' time [approving videos])
//    {
//        request->respond(false); //TODOreq: not dbError/internalError, but lcbOpError=true/%apiCall%Success=false
//        return;
//    }
//    m_DocumentTimelineRegistrationAttemptsApprovalsOrRejectionsDb->setValue(usernameAttemptingToRegister, acceptIfTrue_rejectIfFalse); //TODOreq: when a registration attempt for a desired username fails, that username should be available to attempt registration again (additionally, the registration attempt must be submitted with 24 hours of the declaration of intent to submit registration video?). an alternative is to allow N registration attempts for a given username, but to accept only the first one that meets the requirements (the list of usernames that have been declared intended to be registered should be kept private in that case, else usernames could be jacked right out from under (so see, I kinda like having the declaration of intent to register be a username lock-in (but I also see the game-ability either way!)))
//    if(acceptIfTrue_rejectIfFalse)
//    {
//        QString registrationAttemptJsonString = m_DocumentTimelineRegistrationAttemptsDb->value(usernameAttemptingToRegister).toString(); //never fails, otherwise we shouldn't/wouldn't/won't get here to begin with
//        QJsonDocument registrationAttemptJsonObject = QJsonDocument::fromJson(registrationAttemptJsonString.toUtf8()).object(); //should/will never parse error (so I guess I'm trusting my input saniziting TODOreq?)

//        //I want to take the core registration details (sans vid) and put them in m_DocumentTimelineRegisteredUsersDb, then put the vid itself on the document timeline. Maybe there can/should be some cross-linking between those two (which complicates inline recovery cases, but is doable)... I'm thinking the 'core registration details' should have the key of the timeline doc post of the vid (which, come to think of it, is EASILY calculatable (we can refer to the registration attempt to get the video for calculating the sha1. ahhhhh, but I also don't want to break the one definition/COPY ('copy' emphasized because of large size of videos) rule! I think I'm going in circles again with the 'need to be registered to post' and 'want to post registration as post' chicken-n-egg problem. Another factor is that I want to only use LCB_ADD. I shouldn't/won't (had:can't/won't) LCB_DELETE the registration attempt video, even though I 'could'. This brings about another question: what to do with REJECTED muahaha registration attempts? obviously don't add them to document timeline, but delete them ever? also I think I'm going to want to record my own approving/rejecting SESSIONS (since want to record life 24/7/365 in order to escape paranoia (brainwash could EASILY be weaved (had:injected) into registration attempt submission videos!!!!! ok so I guess that means a document timeline core api change: registration attempts ARE posted by default (as are the declarations?)? Maybe with a flag that is hidden by default (and perhaps that is/should-be the ONLY flag that is hidden by default in the default/frontPage view). So leik both non-registered and registered users have to opt-in to seeing registration attempt videos)))) -- OT: dmca compliance accomodates brainwashing tactics (brainwash uploaded to brain, legally required to delete copyright infringing (or just plain old 'invalid') registration attempt videos, but the damage has already been done (brainwash uploaded to brain) and the evidence destroyed. WHAT THE FUCK? ((a) average joe user could upload copyrighted work X as registration attempt video. (b) evil brainwashing corporation could pretend to upload copyrighted work X as registration attempt video [with embedded/subtle brainwash], then claim copyright infringement to get the evidence deleted). I am torn. Obviously, I must be DMCA compliant for scenario (a)... but being DMCA compliant for scenario (a) means I have to ALLOW sceneario (b). Fuck. I hereby declare copyright law unjust. Fuck you! I'm tempted to say I need a good lawyer, but shouldn't this sound logic be enough????? (should be enough, yes, but probably isn't given the corrupt-as-fuck state of the USA (right this very moment(logically), I wave my hand at suicide, trying to lure me in, like a person waves to a friend they see in a car passing by (this world, rather the world we've made of it, is not logical (living in a non-logical world, to a logical person, qualifies as what religious people call 'hell' (s/Reasonable/Logical (inb4 they are the same thing. They should be, but are not. It should be up to the jury(?) to decide what is logical, perhaps with the guidance of a judge? <--aww shit isn't that how 'reasonable' is determined? There is no solution kill me now (riding this tangent further: when everything said and done: that we rely on human subjectivity/intelligence when processing legal logic is the root of the flaw. It is unethical/immoral to exploit that vulnerability, HOWEVER it becomes necessary to exploit that vulnerability just to be able to fight (or defend) against evil people exploiting the unsolved (unsolvable?) vulnerability for evil purposes. There's the weak argument that exploiting the vulnerability to begin with makes you evil, but there should be an exception for fighting/defending against other exploiters of that vulnerability -- this is all pretty heavy abstract stuff, but applies _DIRECTLY_ to DocumentTimeline because requiremnt #0 of DocumentTimeline is to create a brainwash-proof freedom-of-speech zone. I think I have a solid(had:salad)/sound argument when I say: if the law can't provide you a brainwash-proof freedom-of-speech zone, there's something terribly wrong with the law [that claims to give it's citizens freedom-of-speech]. What's worse: (a) Copyright Infringement (b) Brainwashed Citizens --- Obviously, (b) is much worse... however, the USA might conclude differently seeing as it's controlled by corporations [who are legally required to chase profits] who obviously think Copyright Infringement is worse, seeing as Copyright Infringement decreases their profits. If the actions put into effect by corporate lobbyists are required by law to 'serve the shareholders' (chase profits)... which they are... then [...some observation/conclusion goes here but I'm lost -- but basically this shit is not just (TODOreq figure out this observation because it's needed to defend yourself in court (and is the golden argument))...]))))))))

//        m_DocumentTimelineRegisteredUsersDb->setValue(usernameAttemptingToRegister);
//    }

//    //TODOreq: somewhere around here we'll want to post the registration attempt video on the document timeline as if it was the user's first call to "post". I forget my original point sadly, but it involved the idea of having the registration attempt being a timeline doc (at least in format) all along. I suppose it COULD have a special mimetype added: "timeline-document/new-user-announcement", or it could have a regular mimetype "video/guessed-video-mime" with an additional json tag 'new-user-announcement' : '1' (do json keys require a value?) -- there's a lot of flexibility and idk which is better
//    request->respond(true);
}
void DocumentTimeline::logout(IDocumentTimelineLogoutRequest *request)
{
    qFatal("DocumentTimeline::logout(IDocumentTimelineLogoutRequest *request) not yet implemented");
}
void DocumentTimeline::handleAddToDbFinished(bool dbError, bool addToDbSuccess, void *userData)
{
    IDocumentTimelinePostRequest *request = static_cast<IDocumentTimelinePostRequest*>(userData);
    request->respond(dbError, addToDbSuccess);
}
