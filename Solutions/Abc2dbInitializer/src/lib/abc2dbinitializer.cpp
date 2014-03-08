#include "abc2dbinitializer.h"

#include <QFile>
#include <QTextStream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp> //qt 5 just for json? nahhhhhh

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

//Qt, how I missed thee...
Abc2dbInitializer::Abc2dbInitializer(QObject *parent) :
    QObject(parent), ISynchronousLibCouchbaseUser()
{ }
void Abc2dbInitializer::errorOutput(const string &errorString)
{
    emit d(QString::fromStdString(errorString));
}
void Abc2dbInitializer::initializeAbc2db(const QString &filenameOfLineSeparatedEnormousBitcoinKeyListThatHasAtLeast_110k_keys_ButCanBeMoreIn_10k_key_increments)
{
    if(!connectToCouchbase())
        return;
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
                if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(thisBitcoinKeySetsCouchbaseKey, thisBitcoinKeySetJson, LCB_ADD, 0, outputMsg))
                    return;
                emit d(QString::fromStdString(outputMsg));

                //now make the corresponding 'current page' doc for this set
                pt.clear();
                pt.put(JSON_BITCOIN_KEY_SET_CURRENT_PAGE, "0");
                std::ostringstream oneBitcoinKeySetCurrengPageBuffer;
                write_json(oneBitcoinKeySetCurrengPageBuffer, pt, false);
                std::string thisBitcoinKeySetCurrentPageCouchbaseKey = bitcoinKeySetCurrentPageKey(QString::number(thousandSetsInDbCounter).toStdString());
                std::string thisBitcoinKeySetCurrentPageJson = oneBitcoinKeySetCurrengPageBuffer.str();
                outputMsg = "adding current page doc for bitcoin key set #" + QString::number(thousandSetsInDbCounter).toStdString();
                if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(thisBitcoinKeySetCurrentPageCouchbaseKey, thisBitcoinKeySetCurrentPageJson, LCB_ADD, 0, outputMsg))
                    return;
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
                    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(thisHugeBitcoinKeyListPageCouchbaseKey, thisHugeBitcoinKeyListPageJson, LCB_ADD, 0, outputMsg))
                        return;
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
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(hugeBitcoinKeyListCurrentPageCouchbaseKey, hugeBitcoinKeyListCurrentPageJson, LCB_ADD, 0, outputMsg))
        return;
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
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(campaign0docCouchbaseKey, campaign0docJson, LCB_ADD, 0, outputMsg))
        return;
    emit d(QString::fromStdString(outputMsg));
    emit d("done initializing");
    emit doneInitializingAbc2db();
}
