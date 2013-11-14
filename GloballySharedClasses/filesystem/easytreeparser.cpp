#include "easytreeparser.h"

#include "easytreehashitem.h"

//Returns a list of easy tree hash items. The caller takes ownership of the list returned and also each individual EasyTreeItem therein.
QList<EasyTreeHashItem*> *EasyTreeParser::parseEasyTreeAndReturnAsNewList(QIODevice *easyTree)
{
    bool weOpened = false; //backwards compatibility as I realize during a later project utilizing this library that this is not a good place to do opening/closing
    if(!easyTree->isOpen())
    {
        easyTree->open(QIODevice::ReadOnly | QIODevice::Text);
        weOpened = true;
    }
    QTextStream textStream(easyTree);
    QList<EasyTreeHashItem*> *easyTreeHashList = new QList<EasyTreeHashItem*>();
    while(!textStream.atEnd())
    {
        QString oneLine = textStream.readLine();
        easyTreeHashList->append(EasyTreeHashItem::newEasyTreeHashItemFromLineOfText(oneLine, false, true));
    }
    if(weOpened)
    {
        easyTree->close();
    }
    return easyTreeHashList;
}
