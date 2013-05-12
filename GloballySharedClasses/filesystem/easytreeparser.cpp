#include "easytreeparser.h"

#include "easytreehashitem.h"

QList<EasyTreeHashItem*> *EasyTreeParser::parseEasyTreeAndReturnAsNewList(QIODevice *easyTree)
{
    easyTree->open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream textStream(easyTree);
    QList<EasyTreeHashItem*> *easyTreeHashList = new QList<EasyTreeHashItem*>();
    while(!textStream.atEnd())
    {
        QString oneLine = textStream.readLine();
        easyTreeHashList->append(EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(oneLine));
    }
    easyTree->close();
    return easyTreeHashList;
}
