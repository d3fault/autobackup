#include "antikeyandmouselogger.h"

#include <QDateTime>

AntiKeyAndMouseLogger::AntiKeyAndMouseLogger(QObject *parent)
    : QObject(parent)
{
    qsrand(QDateTime::currentMSecsSinceEpoch()); //TODOreq: boost:random or something more secure. see notes in initial.idea.txt about not using keyboard/mouse entropy
}
void AntiKeyAndMouseLogger::addKeyRangeToListsInclusive(int startKey, int endKey, QList<KeyMapEntry> *firstList, QList<KeyMapEntry> *secondList)
{
    for(int i = startKey; i <= endKey; ++i)
    {
        (*firstList) << i;
        (*secondList) << i;
    }
}
void AntiKeyAndMouseLogger::generateShuffledKeymap()
{
    m_ShuffledKeymap.clear(); //TODOmb: on shutdown/destruct/whatever (or just on clear), I should overwrite the memory. LibCrypo ++has SecMem classes maybe I should use? Oh wait no this app assumes your comp is not compromised duh, otherwise all is [already] lost
    QList<KeyMapEntry> keys;
    QList<KeyMapEntry> values;
    addKeyRangeToListsInclusive(Qt::Key_A, Qt::Key_Z, &keys, &values);
    addKeyRangeToListsInclusive(Qt::Key_0, Qt::Key_9, &keys, &values);

    //TODOmb: add all symbol keys that don't require shift. but eh come to think about it, passwords without special keys work more places. come to think about it some more, some places REQUIRE special keys. hell, some places require capital letters (fuck those places)
    //addKeyToMap(Qt::Key_Comma);

    //shuffle
    while(!keys.isEmpty())
        m_ShuffledKeymap.insert(keys.takeFirst(), values.takeAt(qrand() % values.size()));

    emit shuffledKeymapGenerated(m_ShuffledKeymap);
}
void AntiKeyAndMouseLogger::translateShuffledKeymapEntry(const int &shuffledKeymapEntry)
{
    emit shuffledKeymapEntryTranslated(m_ShuffledKeymap.value(shuffledKeymapEntry)); //TODOmb: error checking? if the frontend gives us a key not in the map, we'd segfault?
}
