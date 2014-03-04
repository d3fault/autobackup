#include "abc2dbinitializer.h"

#include <QFile>
#include <QTextStream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp> //qt 5 just for json? nahhhhhh

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

//TODOoptional: most of these couchbase macros, except ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS (which isn't that fancy anyways, just that pessimistic didn't need it), are a copy/paste job from Abc2PessimisticStateMonitorAndRecoverer. I should put them in some shared "synchronous couchbase" utility class/file ideally...
#define ABCP_DO_RESET_EXPONENTIAL_SLEEP_TIMERS \
m_CurrentExponentialBackoffMicrosecondsAmount = 2500; \
m_CurrentExponentialBackoffSecondsAmount = 0; //5ms is first sleep amount

#define ABCP_DO_ONE_EXPONENTIAL_SLEEP \
if(m_CurrentExponentialBackoffSecondsAmount > 0) \
{ \
    m_CurrentExponentialBackoffSecondsAmount *= 2; \
    sleep(m_CurrentExponentialBackoffSecondsAmount); \
} \
else \
{ \
    m_CurrentExponentialBackoffMicrosecondsAmount *= 2; \
    if(m_CurrentExponentialBackoffMicrosecondsAmount > 1000000) \
    { \
        m_CurrentExponentialBackoffSecondsAmount = 1; \
        sleep(m_CurrentExponentialBackoffSecondsAmount); \
    } \
    else \
    { \
        usleep(m_CurrentExponentialBackoffMicrosecondsAmount); \
    } \
}

#define ABCP_DO_ONE_COUCHBASE_STORE_REQUEST(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
{ \
    lcb_store_cmd_t cmd; \
    const lcb_store_cmd_t *cmds[1]; \
    cmds[0] = &cmd; \
    memset(&cmd, 0, sizeof(cmd)); \
    cmd.v.v0.key = keyStringVar.c_str(); \
    cmd.v.v0.nkey = keyStringVar.length(); \
    cmd.v.v0.bytes = valueStringVar.c_str(); \
    cmd.v.v0.nbytes = valueStringVar.length(); \
    cmd.v.v0.operation = lcbOp; \
    cmd.v.v0.cas = tehCas; \
    error = lcb_store(m_Couchbase, NULL, 1, cmds); \
    if(error != LCB_SUCCESS) \
    { \
        emit d("Failed to set up add request for " + QString::fromStdString(descriptionOfRequestStringLiteral) + QString(lcb_strerror(m_Couchbase, error))); \
        lcb_destroy(m_Couchbase); \
        return; \
    } \
} \
if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS) \
{ \
    emit d("Failed to lcb_wait after set request for " + QString::fromStdString(descriptionOfRequestStringLiteral) + QString(lcb_strerror(m_Couchbase, error))); \
    lcb_destroy(m_Couchbase); \
    return; \
}

#define ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
ABCP_DO_RESET_EXPONENTIAL_SLEEP_TIMERS \
ABCP_DO_ONE_COUCHBASE_STORE_REQUEST(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
while(ABC_COUCHBASE_LCB_ERROR_TYPE_IS_ELIGIBLE_FOR_EXPONENTIAL_BACKOFF(m_LastOpStatus)) \
{ \
    ABCP_DO_ONE_EXPONENTIAL_SLEEP \
    ABCP_DO_ONE_COUCHBASE_STORE_REQUEST(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
}

#define ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF(keyStringVar, valueStringVar, lcbOp, tehCas, descriptionOfRequestStringLiteral) \
if(m_LastOpStatus != LCB_SUCCESS) \
{ \
    emit d("Failed to store " + QString::fromStdString(descriptionOfRequestStringLiteral) + ": " + QString(lcb_strerror(m_Couchbase, m_LastOpStatus))); \
    lcb_destroy(m_Couchbase); \
    return; \
}

//Qt, how I missed thee...
Abc2dbInitializer::Abc2dbInitializer(QObject *parent) :
    QObject(parent)
{ }
void Abc2dbInitializer::initializeAbc2db(const QString &filenameOfLineSeparatedEnormousBitcoinKeyListThatHasAtLeast_110k_keys_ButCanBeMoreIn_10k_key_increments)
{
    struct lcb_create_st createOptions;
    memset(&createOptions, 0, sizeof(createOptions));
    createOptions.v.v0.host = "192.168.56.10:8091"; //TODOreq: supply lots of hosts, either separated by semicolon or comma, I forget..
    lcb_error_t error;
    if((error = lcb_create(&m_Couchbase, &createOptions)) != LCB_SUCCESS)
    {
        emit d("Failed to create a libcouchbase instance: " + QString(lcb_strerror(NULL, error)));
        return;
    }

    lcb_set_cookie(m_Couchbase, this);

    //callbacks
    lcb_set_error_callback(m_Couchbase, Abc2dbInitializer::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, Abc2dbInitializer::configurationCallbackStatic);
    lcb_set_store_callback(m_Couchbase, Abc2dbInitializer::storeCallbackStatic);

    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        emit d("Failed to start connecting libcouchbase instance: " + QString(lcb_strerror(m_Couchbase, error)));
        lcb_destroy(m_Couchbase);
        return;
    }
    if((error = lcb_wait(m_Couchbase)) != LCB_SUCCESS)
    {
        emit d("Failed to lcb_wait after lcb_connect:" + QString(lcb_strerror(m_Couchbase, error)));
        lcb_destroy(m_Couchbase);
        return;
    }
    if(!m_Connected)
    {
        emit d("Failed to connect libcouchbase instance: " + QString(lcb_strerror(m_Couchbase, error)));
        lcb_destroy(m_Couchbase);
        return;
    }

    //connected
    emit d("Connected...");

    QFile enormousBitcoinKeyListFile(filenameOfLineSeparatedEnormousBitcoinKeyListThatHasAtLeast_110k_keys_ButCanBeMoreIn_10k_key_increments);
    if(!enormousBitcoinKeyListFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open '" + filenameOfLineSeparatedEnormousBitcoinKeyListThatHasAtLeast_110k_keys_ButCanBeMoreIn_10k_key_increments + "' for reading");
        return;
    }
    emit d("Reading bitcoin keys...");
    QTextStream enormousBitcoinKeyListTextStream(&enormousBitcoinKeyListFile);
    int hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter = 0;
    int hundredRangesInOneHugeBitcoinKeyListPageCounter = 0;
    int thousandSetsInDbCounter = 0;
    int hugeBitcoinKeyListPageCounter = 0;
    std::string commaSeparated100bitcoinKeysInOneRangeBeingBuilt;
    ptree pt;
    while(!enormousBitcoinKeyListTextStream.atEnd())
    {
        QString currentLine = enormousBitcoinKeyListTextStream.readLine().trimmed();
        if(currentLine.isEmpty())
            continue;
        if(thousandSetsInDbCounter < 1000)
        {
            //filling 1000 sets of 100 keys mode
            pt.put(QString(JSON_BITCOIN_KEY_SET_KEY_PREFIX + QString::number(hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter)).toStdString(), currentLine.toStdString());

            ++hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter;
            if(hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter == 100)
            {
                //time to add this 1 set of 100 keys to db
                std::ostringstream oneBitcoinKeySetOf100BitcoinKeys;
                write_json(oneBitcoinKeySetOf100BitcoinKeys, pt, false);
                std::string thisBitcoinKeySetsCouchbaseKey = bitcoinKeySetPageKey(QString::number(thousandSetsInDbCounter).toStdString(), "0"); //We only make page 0 of each bitcoin key set. The rest are made on demand at runtime by Abc2
                std::string thisBitcoinKeySetJson = oneBitcoinKeySetOf100BitcoinKeys.str();
                std::string outputMsg = "adding bitcoin key set #" + QString::number(thousandSetsInDbCounter).toStdString();
                ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(thisBitcoinKeySetsCouchbaseKey, thisBitcoinKeySetJson, LCB_ADD, 0, outputMsg)
                emit d(QString::fromStdString(outputMsg));

                //now make the corresponding 'current page' doc for this set
                pt.clear();
                pt.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE, "0");
                std::ostringstream oneBitcoinKeySetCurrengPageBuffer;
                write_json(oneBitcoinKeySetCurrengPageBuffer, pt, false);
                std::string thisBitcoinKeySetCurrentPageCouchbaseKey = bitcoinKeySetCurrentPageKey(QString::number(thousandSetsInDbCounter).toStdString());
                std::string thisBitcoinKeySetCurrentPageJson = oneBitcoinKeySetCurrengPageBuffer.str();
                outputMsg = "adding current page doc for bitcoin key set #" + QString::number(thousandSetsInDbCounter).toStdString();
                ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(thisBitcoinKeySetCurrentPageCouchbaseKey, thisBitcoinKeySetCurrentPageJson, LCB_ADD, 0, outputMsg)
                emit d(QString::fromStdString(outputMsg));

                pt.clear();
                hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter = 0;
                ++thousandSetsInDbCounter;
            }
        }
        else
        {
            //using the remaining 10k [increments] to make hugeBitcoinKeyList pages
            if(hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter > 0)
            {
                commaSeparated100bitcoinKeysInOneRangeBeingBuilt += ",";
            }
            commaSeparated100bitcoinKeysInOneRangeBeingBuilt += currentLine.toStdString();

            ++hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter;
            if(hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter == 100)
            {
                //100 keys now in comma separated string, so put it into json and clear the string for the next 100/range
                pt.put(JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_PREFIX + QString::number(hundredRangesInOneHugeBitcoinKeyListPageCounter).toStdString(), commaSeparated100bitcoinKeysInOneRangeBeingBuilt);
                commaSeparated100bitcoinKeysInOneRangeBeingBuilt.clear();
                hundredKeysInOneBitcoinKeySetOrhugeBitcoinKeyListRangeCounter = 0;
                ++hundredRangesInOneHugeBitcoinKeyListPageCounter;
                if(hundredRangesInOneHugeBitcoinKeyListPageCounter == 100)
                {
                    //100 ranges of 100 keys (10k keys) have been added to the pt, so now store it and reset everything to handle more 10k increments
                    std::ostringstream oneHugeBitcoinKeyListPageOfTenThousandKeys;
                    write_json(oneHugeBitcoinKeyListPageOfTenThousandKeys, pt, false);
                    std::string thisHugeBitcoinKeyListPageCouchbaseKey = hugeBitcoinKeyListPageKey(QString::number(hugeBitcoinKeyListPageCounter).toStdString());
                    std::string thisHugeBitcoinKeyListPageJson = oneHugeBitcoinKeyListPageOfTenThousandKeys.str();
                    std::string outputMsg = "adding huge bitcoin key list page #" + QString::number(hugeBitcoinKeyListPageCounter).toStdString();
                    ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(thisHugeBitcoinKeyListPageCouchbaseKey, thisHugeBitcoinKeyListPageJson, LCB_ADD, 0, outputMsg)
                    emit d(QString::fromStdString(outputMsg));

                    pt.clear();
                    hundredRangesInOneHugeBitcoinKeyListPageCounter = 0;
                    ++hugeBitcoinKeyListPageCounter;
                }
            }
        }
    }

    if(pt.size() != 0)
    {
        emit d("You didn't do 10k increments, and the system demands that you do! Erase the db contents and try again with perfect 10k increments (110k keys minimum)");
        return;
    }

    //pt.put(JSON_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY, QString::number(hugeBitcoinKeyListPageCounter-1)); //oops, we start at 0 on this one too...
    pt.put(JSON_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY, "0");
    std::ostringstream hugeBitcoinKeyListCurrentPageBuffer;
    write_json(hugeBitcoinKeyListCurrentPageBuffer, pt, false);
    std::string hugeBitcoinKeyListCurrentPageCouchbaseKey = hugeBitcoinKeyListCurrentPageKey(); //derp
    std::string hugeBitcoinKeyListCurrentPageJson = hugeBitcoinKeyListCurrentPageBuffer.str();
    std::string outputMsg = "adding huge bitcoin key list current page";
    ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(hugeBitcoinKeyListCurrentPageCouchbaseKey, hugeBitcoinKeyListCurrentPageJson, LCB_ADD, 0, outputMsg)
    emit d(QString::fromStdString(outputMsg));

    emit d("done reading file, you should have seen 1000 sets created and at least one huge bitcoin key list page created (and one more for every 10k increment on top of that)");

    //campaign 0 doc
    pt.clear();
    pt.put(JSON_AD_SPACE_CAMPAIGN_MIN_PRICE, "1");
    pt.put(JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS, "24");
    std::ostringstream campaign0docBuffer;
    write_json(campaign0docBuffer, pt, false);
    std::string campaign0docCouchbaseKey = adSpaceCampaignKey("d3fault", "0");
    std::string campaign0docJson = campaign0docBuffer.str();
    outputMsg = "adding campaign 0 doc";
    ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(campaign0docCouchbaseKey, campaign0docJson, LCB_ADD, 0, outputMsg)
    emit d(QString::fromStdString(outputMsg));
    emit d("done initializing");
    emit doneInitializingAbc2db();
}
void Abc2dbInitializer::errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    const_cast<Abc2dbInitializer*>(static_cast<const Abc2dbInitializer*>(lcb_get_cookie(instance)))->errorCallback(error, errinfo);
}
void Abc2dbInitializer::errorCallback(lcb_error_t error, const char *errinfo)
{
    emit d("Got an error in our couchbase error callback: " + QString(lcb_strerror(m_Couchbase, error)) + " / " + errinfo);
}
void Abc2dbInitializer::configurationCallbackStatic(lcb_t instance, lcb_configuration_t config)
{
    const_cast<Abc2dbInitializer*>(static_cast<const Abc2dbInitializer*>(lcb_get_cookie(instance)))->configurationCallback(config);
}
void Abc2dbInitializer::configurationCallback(lcb_configuration_t config)
{
    if(config == LCB_CONFIGURATION_NEW)
    {
        m_Connected = true;
    }
}
void Abc2dbInitializer::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    const_cast<Abc2dbInitializer*>(static_cast<const Abc2dbInitializer*>(lcb_get_cookie(instance)))->storeCallback(cookie, operation, error, resp);
}
void Abc2dbInitializer::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)cookie;
    (void)operation;
    (void)resp;
    m_LastOpStatus = error;
}
