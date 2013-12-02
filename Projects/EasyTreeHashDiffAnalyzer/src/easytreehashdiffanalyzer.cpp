#include "easytreehashdiffanalyzer.h"

//QString EasyTreeHashDiffAnalyzer::DirectoryString = "Directory";
//QString EasyTreeHashDiffAnalyzer::FileString = "File";
const QString EasyTreeHashDiffAnalyzer::SpaceString = " ";
const QString EasyTreeHashDiffAnalyzer::ColonString = ":";
const QString EasyTreeHashDiffAnalyzer::MismatchString = "Mismatch";
const QString EasyTreeHashDiffAnalyzer::ExistenceString = "Existence";
const QString EasyTreeHashDiffAnalyzer::OnlyString = "Only";
const QString EasyTreeHashDiffAnalyzer::FileSizeString = "FileSize";
const QString EasyTreeHashDiffAnalyzer::HashString = "Hash";
const QString EasyTreeHashDiffAnalyzer::VersusString = "vs.";
const QString EasyTreeHashDiffAnalyzer::EqualsString = "=";
const QString EasyTreeHashDiffAnalyzer::Input1String = "Input1";
const QString EasyTreeHashDiffAnalyzer::Input2String = "Input2";

EasyTreeHashDiffAnalyzer::EasyTreeHashDiffAnalyzer(QObject *parent) :
    QObject(parent)
{
}
void EasyTreeHashDiffAnalyzer::analyzeEasyTreeHashesAndReportDifferences(QIODevice *easyTreeHash1, QIODevice *easyTreeHash2)
{
    QList<EasyTreeHashItem*> *easyTreeHashList1 = EasyTreeParser::parseEasyTreeAndReturnAsNewList(easyTreeHash1, false);
    QList<EasyTreeHashItem*> *easyTreeHashList2 = EasyTreeParser::parseEasyTreeAndReturnAsNewList(easyTreeHash2, false);

    //check for files that exist in 1 but not 2 -- and as an optimization remove ones whose filesize and hash matches (saves iterations later)
    int easyTreeHashList1Length = easyTreeHashList1->length();

    EasyTreeHashItem *currentItem1;
    EasyTreeHashItem *currentItem2;

    //TODOoptimization: I could optimize this method further by reading in only list2 fully into memory (like i already do above (but for both atm. after the optimization it would just be 1 list read in full like that))and then reading list1 directly from the IO device and merging the IO device reading with the below nested for loop. another optimization that goes with it is asynchronous reading of the next line of the iodevice (list1) while i'm iterating/searching list2 for the line that was just read. it is a good optimization because one is cpu bound and the other is io bound. the cpu bound search will almost always beat the io bound readNextLine (and if it doesn't, perhaps we need a larger buffer somewhere (or more specifically, we need to use a buffer somewhere where we aren't yet))

    for(int i = 0; i < easyTreeHashList1Length; ++i)
    {
        currentItem1 = easyTreeHashList1->at(i);
        bool seenIn2 = false;
        int easyTreeHashList2Length = easyTreeHashList2->length();
        for(int j = 0; j < easyTreeHashList2Length; ++j)
        {
            currentItem2 = easyTreeHashList2->at(j);
            if(currentItem1->relativeFilePath() == currentItem2->relativeFilePath())
            {
                seenIn2 = true;

                if(!currentItem1->isDirectory())
                {
                    if(currentItem1->fileSize() != currentItem2->fileSize())
                    {
                        emit d(MismatchString + ColonString + SpaceString + currentItem1->relativeFilePath() + ColonString + SpaceString + FileSizeString + ColonString + SpaceString + Input1String + EqualsString + QString::number(currentItem1->fileSize()) + SpaceString + VersusString + SpaceString + Input2String + EqualsString + QString::number(currentItem2->fileSize()));
                    }
                    else if(currentItem1->hasHash() && (currentItem1->hash() != currentItem2->hash()))
                    {
                        emit d(MismatchString + ColonString + SpaceString + currentItem1->relativeFilePath() + ColonString + SpaceString + HashString + ColonString + SpaceString + Input1String + EqualsString + QString(currentItem1->hash()) + SpaceString + VersusString + SpaceString + Input2String + EqualsString + QString(currentItem2->hash()));
                    }
                }

                easyTreeHashList2->removeAt(j); //MATCH (or mismatch) (so remove from comparing 2 against 1)

                delete currentItem2; //inline cleanup, first place we know we don't need currentItem2
            }
            if(seenIn2)
            {
                break; //already saw it in list 2, so no need to continue looping it
            }
        }
        if(!seenIn2)
        {
            emit d(MismatchString + ColonString + SpaceString + currentItem1->relativeFilePath() + ColonString + SpaceString + ExistenceString + ColonString + SpaceString + Input1String + SpaceString + OnlyString);
        }

        delete currentItem1; //inline cleanup
    }

    //after the above optimized loop, the only stuff left in 2 is the stuff not in 1...
    int easyTreeHashList2Length = easyTreeHashList2->length();
    for(int i = 0; i < easyTreeHashList2Length; ++i)
    {
        currentItem2 = easyTreeHashList2->at(i);
        emit d(MismatchString + ColonString + SpaceString + currentItem2->relativeFilePath() + ColonString + SpaceString + ExistenceString + ColonString + SpaceString + Input2String + SpaceString + OnlyString);

        delete currentItem2; //inline cleanup
    }
    delete easyTreeHashList1;
    delete easyTreeHashList2;
    emit d("Done Anaylzing Differences");
}
