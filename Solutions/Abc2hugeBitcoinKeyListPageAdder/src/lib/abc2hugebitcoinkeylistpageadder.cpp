#include "abc2hugebitcoinkeylistpageadder.h"

#include <QFile>
#include <QTextStream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

//macros copy/paste job from pessimistic state recoverer and/or initializer
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

Abc2hugeBitcoinKeyListPageAdder::Abc2hugeBitcoinKeyListPageAdder(QObject *parent) :
    QObject(parent)
{ }
void Abc2hugeBitcoinKeyListPageAdder::addHugeBitcoinKeyListPages(const QString &nonExistentStartingPageIndex, const QString &filenameOfBitcoinKeysIn10kIncrements)
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
    lcb_set_error_callback(m_Couchbase, Abc2hugeBitcoinKeyListPageAdder::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, Abc2hugeBitcoinKeyListPageAdder::configurationCallbackStatic);
    lcb_set_store_callback(m_Couchbase, Abc2hugeBitcoinKeyListPageAdder::storeCallbackStatic);

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

    QFile inputBitcoinKeyListPagesFile(filenameOfBitcoinKeysIn10kIncrements);
    if(!inputBitcoinKeyListPagesFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit d("failed to open '" + filenameOfBitcoinKeysIn10kIncrements + "' for reading");
        return;
    }
    emit d("Reading bitcoin keys...");
    QTextStream inputBitcoinKeyListPagesTextStream(&inputBitcoinKeyListPagesFile);
    int hundredKeysInHugeBitcoinKeyListRangeCounter = 0;
    int hundredRangesInOneHugeBitcoinKeyListPageCounter = 0;
    std::string commaSeparated100bitcoinKeysInOneRangeBeingBuilt;
    std::string currentPage = nonExistentStartingPageIndex.toStdString();
    ptree pt;
    while(!inputBitcoinKeyListPagesTextStream.atEnd())
    {
        QString currentLine = inputBitcoinKeyListPagesTextStream.readLine().trimmed();
        if(currentLine.isEmpty())
            continue;
        /////////////////////begin what is basically copy paste job from initializer (but uses different page counter)//////////
        if(hundredKeysInHugeBitcoinKeyListRangeCounter > 0)
        {
            commaSeparated100bitcoinKeysInOneRangeBeingBuilt += ",";
        }
        commaSeparated100bitcoinKeysInOneRangeBeingBuilt += currentLine.toStdString();

        ++hundredKeysInHugeBitcoinKeyListRangeCounter;
        if(hundredKeysInHugeBitcoinKeyListRangeCounter == 100)
        {
            //100 keys now in comma separated string, so put it into json and clear the string for the next 100/range
            pt.put(JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_PREFIX + QString::number(hundredRangesInOneHugeBitcoinKeyListPageCounter).toStdString(), commaSeparated100bitcoinKeysInOneRangeBeingBuilt);
            commaSeparated100bitcoinKeysInOneRangeBeingBuilt.clear();
            hundredKeysInHugeBitcoinKeyListRangeCounter = 0;
            ++hundredRangesInOneHugeBitcoinKeyListPageCounter;
            if(hundredRangesInOneHugeBitcoinKeyListPageCounter == 100)
            {
                //100 ranges of 100 keys (10k keys) have been added to the pt, so now store it and reset everything to handle more 10k increments
                std::ostringstream oneHugeBitcoinKeyListPageOfTenThousandKeys;
                write_json(oneHugeBitcoinKeyListPageOfTenThousandKeys, pt, false);
                std::string thisHugeBitcoinKeyListPageCouchbaseKey = hugeBitcoinKeyListPageKey(currentPage);
                std::string thisHugeBitcoinKeyListPageJson = oneHugeBitcoinKeyListPageOfTenThousandKeys.str();
                std::string outputMsg = "adding huge bitcoin key list page #" + currentPage;
                ABCP_DO_COUCHBASE_STORE_REQUEST_WITH_EXPONENTIAL_BACKOFF_REQUIRING_LCB_SUCCESS(thisHugeBitcoinKeyListPageCouchbaseKey, thisHugeBitcoinKeyListPageJson, LCB_ADD, 0, outputMsg)
                emit d(QString::fromStdString(outputMsg));

                pt.clear();
                hundredRangesInOneHugeBitcoinKeyListPageCounter = 0;

                //++currentPage;
                bool convertOk;
                currentPage = QString::number(QString::fromStdString(currentPage).toInt(&convertOk)+1).toStdString(); //LOL
            }
        }
        /////////////////////end what is basically copy paste job from initializer//////////
    }
    if(pt.size() != 0)
    {
        emit d("You didn't do 10k increments, and the system demands that you do! Grats, you just fucking wrecked your system. Have fun duct taping it back together manually. You have a while before it segfaults trying to read a json key (bitcoin key range) that isn't there");
        return;
    }
    emit d("done adding pages, you should have seen one huge bitcoin key list page created for every 10k increment");
    emit doneAddingHugeBitcoinKeyListPages();
}
void Abc2hugeBitcoinKeyListPageAdder::errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    const_cast<Abc2hugeBitcoinKeyListPageAdder*>(static_cast<const Abc2hugeBitcoinKeyListPageAdder*>(lcb_get_cookie(instance)))->errorCallback(error, errinfo);
}
void Abc2hugeBitcoinKeyListPageAdder::errorCallback(lcb_error_t error, const char *errinfo)
{
    emit d("Got an error in our couchbase error callback: " + QString(lcb_strerror(m_Couchbase, error)) + " / " + errinfo);
}
void Abc2hugeBitcoinKeyListPageAdder::configurationCallbackStatic(lcb_t instance, lcb_configuration_t config)
{
    const_cast<Abc2hugeBitcoinKeyListPageAdder*>(static_cast<const Abc2hugeBitcoinKeyListPageAdder*>(lcb_get_cookie(instance)))->configurationCallback(config);
}
void Abc2hugeBitcoinKeyListPageAdder::configurationCallback(lcb_configuration_t config)
{
    if(config == LCB_CONFIGURATION_NEW)
    {
        m_Connected = true;
    }
}
void Abc2hugeBitcoinKeyListPageAdder::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    const_cast<Abc2hugeBitcoinKeyListPageAdder*>(static_cast<const Abc2hugeBitcoinKeyListPageAdder*>(lcb_get_cookie(instance)))->storeCallback(cookie, operation, error, resp);
}
void Abc2hugeBitcoinKeyListPageAdder::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    (void)cookie;
    (void)operation;
    (void)resp;
    m_LastOpStatus = error;
}

