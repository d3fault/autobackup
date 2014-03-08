#include "abc2hugebitcoinkeylistpageadder.h"

#include <QFile>
#include <QTextStream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "abc2couchbaseandjsonkeydefines.h"

using namespace boost::property_tree;

Abc2hugeBitcoinKeyListPageAdder::Abc2hugeBitcoinKeyListPageAdder(QObject *parent) :
    QObject(parent), ISynchronousLibCouchbaseUser()
{ }
void Abc2hugeBitcoinKeyListPageAdder::errorOutput(const string &errorString)
{
    emit d(QString::fromStdString(errorString));
}
void Abc2hugeBitcoinKeyListPageAdder::addHugeBitcoinKeyListPages(const QString &nonExistentStartingPageIndex, const QString &filenameOfBitcoinKeysIn10kIncrements)
{
    if(!connectToCouchbase())
        return;
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
                if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(thisHugeBitcoinKeyListPageCouchbaseKey, thisHugeBitcoinKeyListPageJson, LCB_ADD, 0, outputMsg))
                    return;
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
