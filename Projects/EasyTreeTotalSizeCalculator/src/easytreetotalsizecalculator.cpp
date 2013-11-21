#include "easytreetotalsizecalculator.h"

EasyTreeTotalSizeCalculator::EasyTreeTotalSizeCalculator(QObject *parent) :
    QObject(parent)
{
}
void EasyTreeTotalSizeCalculator::calculateTotalSizeOfEasyTreeEntries(const QString &filepathOfEasyTreeFile, bool easyTreeFileIsReplacementsFormatted)
{
    if(!QFile::exists(filepathOfEasyTreeFile))
    {
        emit d("The filename supplied to easy tree total size calculator does not exist");
        return;
    }

    QFile easyTreeFile(filepathOfEasyTreeFile);
    easyTreeFile.open(QIODevice::ReadOnly | QIODevice::Text);
    calculateTotalSizeOfEasyTreeEntries(&easyTreeFile, easyTreeFileIsReplacementsFormatted);
    easyTreeFile.close();
}
void EasyTreeTotalSizeCalculator::calculateTotalSizeOfEasyTreeEntries(QIODevice *ioDeviceOfEasyTreeFile, bool easyTreeFileIsReplacementsFormatted)
{
    QList<EasyTreeHashItem*> *easyTreeHashList = EasyTreeParser::parseEasyTreeAndReturnAsNewList(ioDeviceOfEasyTreeFile, easyTreeFileIsReplacementsFormatted);

    qint64 calcualtedFilesSize = 0.0;
    EasyTreeHashItem *currentItem;
    QListIterator<EasyTreeHashItem*> *it = new QListIterator<EasyTreeHashItem*>(*easyTreeHashList);
    while(it->hasNext())
    {
        currentItem = it->next();
        if(!currentItem->isDirectory())
        {
            calcualtedFilesSize += currentItem->fileSize(); //fuck yea dis is some crazy complex code!
        }
        delete currentItem; //inline cleanup
    }
    emit d(QString("Calculated Total Size of Easy Tree Entries in Bytes: ") + QString::number(calcualtedFilesSize));
    emit d(QString("Calculated Total Size of Easy Tree Entries in KiloBytes: ") + QString::number(calcualtedFilesSize/1024.0));
    emit d(QString("Calculated Total Size of Easy Tree Entries in MegaBytes: ") + QString::number((calcualtedFilesSize/1024.0)/1024.0));
    emit d(QString("Calculated Total Size of Easy Tree Entries in GigaBytes: ") + QString::number(((calcualtedFilesSize/1024.0)/1024.0)/1024.0));



    delete it;
    delete easyTreeHashList;
}
