#ifndef GETTODAYSADSLOTSERVERCLIENTCONNECTION_H
#define GETTODAYSADSLOTSERVERCLIENTCONNECTION_H

#include <QObject>
#include <QTextStream>

#include <event2/event.h>

#include <boost/lockfree/queue.hpp>
using namespace boost::lockfree;

#include "../frontend2backendRequests/abcapigetcouchbasedocumentbykeyrequestresponder.h"
#include "../frontend2backendRequests/abcapistorecouchbasedocumentbykeyrequestresponder.h"

class QTcpSocket;
class GetCouchbaseDocumentByKeyRequest;
class StoreCouchbaseDocumentByKeyRequest;

class GetTodaysAdSlotServerClientConnection : public QObject
{
    Q_OBJECT
public:
    static void setBackendGetQueue(queue<GetCouchbaseDocumentByKeyRequest*> *backendQueue);
    static void setBackendStoreQueue(queue<StoreCouchbaseDocumentByKeyRequest*> *backendQueue);
    static void setBackendGetQueueEvent(struct event *backendQueueEvent);
    static void setBackendStoreQueueEvent(struct event *backendQueueEvent);

    explicit GetTodaysAdSlotServerClientConnection(QTcpSocket *clientSocket, QObject *parent = 0);
    ~GetTodaysAdSlotServerClientConnection();
private:
    enum GetTodaysAdSlotServerClientConnectionStageEnum
    {
          GetAdCampaignCurrentSlotCacheIfExists = 0
        , GetAdCampaignDocItselfJustToGetSlotLengthHours = 1
        , GetAdCampaignSlot = 2
        , GetAdCampaignSlotFiller = 3
        , CreateOrUpdateAdCampaignCurrentSlotCache = 4
    };
    static AbcApiGetCouchbaseDocumentByKeyRequestResponder s_GetResponder;
    static AbcApiStoreCouchbaseDocumentByKeyRequestResponder s_StoreResponder;
    static queue<GetCouchbaseDocumentByKeyRequest*> *s_BackendGetQueue;
    static queue<StoreCouchbaseDocumentByKeyRequest*> *s_BackendStoreQueue;
    static struct event *s_BackendGetQueueEvent;
    static struct event *s_BackendStoreQueueEvent;

    QTextStream m_ClientStream;

    GetTodaysAdSlotServerClientConnectionStageEnum m_Stage;
    std::string m_CampaignOwnerRequested;
    std::string m_CampaignIndexRequested;
    bool m_FoundAdCampaignCurrentSlotCacheDoc;
    std::string m_FirstSlotIndexToTry;
    std::string m_CurrentSlotIndexToTry;
    int m_SlotLengthHours;
    long long m_CurrentAdSlotExpireDateTime;

    bool m_BackendRequestPending;
    bool m_WantsToBeDeletedLater;

    void beginDbGet(const std::string &keyToGet, GetTodaysAdSlotServerClientConnectionStageEnum stageToSet);
    void beginDbStore(const std::string &keyToStore, const std::string &documentToStore, GetTodaysAdSlotServerClientConnectionStageEnum stageToSet);
    void sendResponseAndCloseSocket(const QString &response);

    void continueAtJustFinishedAttemptingToGetAdCampaignCurrentSlotCache(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void continueAtJustFinishedAttemptingToGetAdCampaign(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void haveSlotLengthHoursAndKnowFirstSlotIndexToTrySoTry();
    void tryToGetCurrentSlotIndex();
    void continueAtJustFinishedAttemptingToGetAdCampaignSlot(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void continueAtJustFinishedAttemptingToGetAdCampaignSlotFiller(const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
public slots:
    void backendDbGetCallback(std::string couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void backendDbStoreCallback(bool lcbOpSuccess, bool dbError);
private slots:
    void handleClientReadyRead();
    void deleteLaterIfNoBackendRequestPendingOrWhenThatBackendRequestFinishes();
};

#endif // GETTODAYSADSLOTSERVERCLIENTCONNECTION_H
