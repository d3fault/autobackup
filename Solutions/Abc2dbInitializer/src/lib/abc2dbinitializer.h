#ifndef ABC2DBINITIALIZER_H
#define ABC2DBINITIALIZER_H

#include <QObject>

#include <libcouchbase/couchbase.h>

class Abc2dbInitializer : public QObject
{
    Q_OBJECT
public:
    explicit Abc2dbInitializer(QObject *parent = 0);
private:
    lcb_t m_Couchbase;
    lcb_error_t m_LastOpStatus;
    bool m_Connected;
    std::string m_LastDocGetted;
    lcb_cas_t m_LastGetCas;
    __useconds_t m_CurrentExponentialBackoffMicrosecondsAmount;
    unsigned int m_CurrentExponentialBackoffSecondsAmount;

    static void errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo);
    void errorCallback(lcb_error_t error, const char *errinfo);
    static void configurationCallbackStatic(lcb_t instance, lcb_configuration_t config);
    void configurationCallback(lcb_configuration_t config);
    static void getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    void getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp);
    static void storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
    void storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp);
signals:
    void d(const QString &);
    void doneInitializingAbc2db();
public slots:
    void initializeAbc2db(const QString &filenameOfLineSeparatedEnormousBitcoinKeyListThatHasAtLeast_110k_keys_ButCanBeMoreIn_10k_key_increments);
};

#endif // ABC2DBINITIALIZER_H
