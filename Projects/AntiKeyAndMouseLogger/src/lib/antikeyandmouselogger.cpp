#include "antikeyandmouselogger.h"

QList<KeyMapEntry> AntiKeyAndMouseLogger::m_EntrySelectionKeys = allTypeableOnUsKeyboardWithoutNeedingShiftKey();
QList<KeyMapEntry> AntiKeyAndMouseLogger::m_NonShuffledKeymap = allTypeableKeysOnUsKeyboard();

#include <QDateTime>

#define AntiKeyAndMouseLogger_NEXT_PAGE_SPECIAL_SYMBOL Qt::Key_copyright

//TODOmb: qwerty? I have width/height restrictions on my 1.8" monitor (160x128), but eh could always scale the font size down anyways
//TODOreq: if I implement "clear clipboard after 30 seconds", I should make sure that what I'm clearing is [still] there password. We don't want to clear it if it isn't. I wonder if KeePass does this check O_o (could find out right now but fuck it), I should file a bug if not
//TODOreq: if they keep pressing "next page" so many times that we wrap around to the first page again, we should reshuffle all the pages before showing the first page. Otherwise a keylogger could determine which keys are the 'next page' keys for a given set of pages.... which blah probably tells them NOTHING but eh just to err on the side of caution fuck it
AntiKeyAndMouseLogger::AntiKeyAndMouseLogger(QObject *parent)
    : QObject(parent)
{
    qsrand(QDateTime::currentMSecsSinceEpoch()); //TODOreq: boost:random or something more secure. see notes in initial.idea.txt about not using keyboard/mouse entropy
}
QList<KeyMapEntry> AntiKeyAndMouseLogger::allTypeableOnUsKeyboardWithoutNeedingShiftKey()
{
    QList<KeyMapEntry> ret;
    ret << "`";
    for(int i = Qt::Key_1; i <= Qt::Key_9; ++i)
        ret << QtKeyToString(i);
    ret << "0" << "-" << "=";
    ret << "q" << "w" << "e" << "r" << "t" << "y" << "u" << "i" << "o" << "p" << "[" << "]" << "\\" << "a" << "s" << "d" << "f" << "g" << "h" << "j" << "k" << "l" << ";" << "'" << "z" << "x" << "c" << "v" << "b" << "n" << "m" << "," << "." << "/";
    return ret;
}
QList<KeyMapEntry> AntiKeyAndMouseLogger::allTypeableKeysOnUsKeyboard()
{
    QList<KeyMapEntry> ret = allTypeableOnUsKeyboardWithoutNeedingShiftKey();
    ret << "~" << "!" << "@" << "#" << "$" << "%" << "^" << "&" << "*" << "(" << ")" << "_" << "+" << "Q" << "W" << "E" << "R" << "T" << "Y" << "U" << "I" << "O" << "P" << "{" << "}" << "|" << "A" << "S" << "D" << "F" << "G" << "H" << "J" << "K" << "L" << ":" << "\"" << "Z" << "X" << "C" << "V" << "B" << "N" << "M" << "<" << ">" << "?";
    return ret;
}
int AntiKeyAndMouseLogger::numEntriesOnOneKeymapPage()
{
    return m_EntrySelectionKeys.size(); //soon this would be size/2 (for "non-shift" and "shift" pages), but ultimately it could be arbitrary/run-time-specifiable-depending-on-screen-size-etc and this app could cover much more (all?) of the unicode keymap
}
void AntiKeyAndMouseLogger::generateShuffledKeymapAndRequestPresentationOfFirstPage()
{
    m_ShuffledKeymap.clear(); //TODOmb: on shutdown/destruct/whatever (or just on clear), I should overwrite the memory. LibCrypo ++has SecMem classes maybe I should use? Oh wait no this app assumes your comp is not compromised duh, otherwise all is [already] lost

    QList<KeyMapEntry> values = m_NonShuffledKeymap;
    Q_FOREACH(const KeyMapEntry &currentEnterableKey, m_EntrySelectionKeys)
    {
        if(values.isEmpty())
            break;
        m_ShuffledKeymap.append(qMakePair(currentEnterableKey, values.takeAt(qrand() % values.size())));
    }

    emit presentShuffledKeymapPageRequested(m_ShuffledKeymap);
}
void AntiKeyAndMouseLogger::translateShuffledKeymapEntry(const QString &shuffledKeymapEntry)
{
    Q_FOREACH(const KeymapHashTypes &currentEntry, m_ShuffledKeymap)
    {
        if(shuffledKeymapEntry == currentEntry.first)
        {
            emit shuffledKeymapEntryTranslated(currentEntry.second);
            generateShuffledKeymapAndRequestPresentationOfFirstPage();
            return;
        }
    }
    //TODOmb: error checking? if the frontend gives us a key not in the map, we'd become unusable because of a lack of a signal response here
}
