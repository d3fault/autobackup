#ifndef EASYTREEPARSER_H
#define EASYTREEPARSER_H

#include <QObject>
#include <QIODevice>

class EasyTreeHashItem;
class EasyTreeParser : public QObject
{
    Q_OBJECT
public:
    static QList<EasyTreeHashItem*> *parseEasyTreeAndReturnAsNewList(QIODevice* easyTree, bool easyTreeFileIsReplacementsFormatted = false);
};

#endif // EASYTREEPARSER_H
