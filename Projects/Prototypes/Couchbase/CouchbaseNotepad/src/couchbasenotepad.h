#ifndef COUCHBASENOTEPAD_H
#define COUCHBASENOTEPAD_H

#include <QObject>

#include <libcouchbase/couchbase.h>
#include <event2/event.h>

#define ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE 1

class CouchbaseNotepad : public QObject
{
    Q_OBJECT
public:
    explicit CouchbaseNotepad(QObject *parent = 0);
private:
    lcb_t m_Couchbase;

    static const struct timeval m_ThirtyMilliseconds;
    struct event *m_ProcessQtEventsTimeout;

    unsigned int m_PendingGetCount;
    unsigned int m_PendingStoreCount;

#if 0 //eh, unneeded?
    struct event *m_GetEventCallback;
    struct event *m_StoreEventCallback;
#endif


    static void errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo);
    void errorCallback(lcb_error_t error, const char *errinfo);

    static void configurationCallbackStatic(lcb_t instance, lcb_configuration_t config);
    void configurationCallback(lcb_configuration_t config);

    static void storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void storeCallbackActual(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);

    static void getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void getCallbackActual(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);

#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
    static void durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp);
    void durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp);
#endif

#if 0 //eh, unneeded?
    static void libEventSlotForGetStatic(evutil_socket_t unusedSocket, short events, void *userData);
    static void libEventSlotForStoreStatic(evutil_socket_t unusedSocket, short events, void *userData);
#endif

#if 0 //later
    void scheduleGetRequest(AutoRetryingWithExponentialBackoffCouchbaseGetRequest *autoRetryingWithExponentialBackoffCouchbaseGetRequest);
    void scheduleStoreRequest(AutoRetryingWithExponentialBackoffCouchbaseStoreRequest *autoRetryingWithExponentialBackoffCouchbaseStoreRequest);
#endif

    void libEventSlotForGet();
    void libEventSlotForStore();

    static void callQAppProcessEventsStatic(evutil_socket_t unusedSocket, short events, void *userData);
    void callQAppProcessEvents();
signals:
    void e(const QString &msg);

    void initializedAndStartedSuccessfully();

    void getCouchbaseNotepadDocFinished(bool dbError, bool lcbOpSuccess, const QString &key, const QString &value);
    void addCouchbaseNotepadDocFinished(bool dbError, bool lcbOpSuccess, const QString &key, const QString &value);

    void couchbaseNotepadExitted(bool exittedCleanly = false);
public slots:
    void initializeAndStart();

    void getCouchbaseNotepadDocByKey(const QString &key);
    void storeCouchbaseNotepadDocByKey(const QString &key, const QString &value, bool overwriteExisting = false);
};

#endif // COUCHBASENOTEPAD_H
