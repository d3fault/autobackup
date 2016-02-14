#include "antikeyandmouselogger.h"

QList<KeyMapEntry> AntiKeyAndMouseLogger::m_NonShuffledKeymap = staticInitNonShuffledKeymap();

#include <QDateTime>

//TODOmb: qwerty? I have width/height restrictions on my 1.8" monitor (160x128), but eh could always scale the font size down anyways
AntiKeyAndMouseLogger::AntiKeyAndMouseLogger(QObject *parent)
    : QObject(parent)
{
    qsrand(QDateTime::currentMSecsSinceEpoch()); //TODOreq: boost:random or something more secure. see notes in initial.idea.txt about not using keyboard/mouse entropy
}
QList<KeyMapEntry> AntiKeyAndMouseLogger::staticInitNonShuffledKeymap()
{
    QList<KeyMapEntry> ret;
    //TO DOnereq: figure out how to initialize a QList statically PROPERLY xD, isuck.jpg
    for(int i = Qt::Key_A; i <= Qt::Key_Z; ++i)
        m_NonShuffledKeymap << i;
    for(int i = Qt::Key_0; i <= Qt::Key_9; ++i)
        m_NonShuffledKeymap << i;
    return ret;
}
int AntiKeyAndMouseLogger::numEntriesOnOneKeymapPage()
{
    return m_NonShuffledKeymap.size(); //soon this would be size/2 (for "non-shift" and "shift" pages), but ultimately it could be arbitrary/run-time-specifiable-depending-on-screen-size-etc and this app could cover much more (all?) of the unicode keymap
}
void AntiKeyAndMouseLogger::generateShuffledKeymap()
{
    m_ShuffledKeymap.clear(); //TODOmb: on shutdown/destruct/whatever (or just on clear), I should overwrite the memory. LibCrypo ++has SecMem classes maybe I should use? Oh wait no this app assumes your comp is not compromised duh, otherwise all is [already] lost
    QList<KeyMapEntry> keys = m_NonShuffledKeymap;
    QList<KeyMapEntry> values = m_NonShuffledKeymap;

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
    generateShuffledKeymap();
}
