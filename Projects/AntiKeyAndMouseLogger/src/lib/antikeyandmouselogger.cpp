#include "antikeyandmouselogger.h"

QList<KeyMapEntry> AntiKeyAndMouseLogger::m_EntrySelectionKeys = allTypeableOnUsKeyboardWithoutNeedingShiftKey();
QList<KeyMapEntry> AntiKeyAndMouseLogger::m_NonShuffledKeymap = allTypeableKeysOnUsKeyboard();

#include <QDateTime>

//TODOmb: qwerty? I have width/height restrictions on my 1.8" monitor (160x128), but eh could always scale the font size down anyways
//TODOreq: if I implement "clear clipboard after 30 seconds", I should make sure that what I'm clearing is [still] there password. We don't want to clear it if it isn't. I wonder if KeePass does this check O_o (could find out right now but fuck it), I should file a bug if not
//TODOreq: if they keep pressing "next page" so many times that we wrap around to the first page again, we should reshuffle all the pages before showing the first page. Otherwise a keylogger could determine which keys are the 'next page' keys for a given set of pages.... which blah probably tells them NOTHING but eh just to err on the side of caution fuck it
AntiKeyAndMouseLogger::AntiKeyAndMouseLogger(QObject *parent)
    : QObject(parent)
    , m_CurrentShuffledKeymapPageIndex(0)
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
    QList<KeyMapEntry> ret = m_EntrySelectionKeys;
    ret << "~" << "!" << "@" << "#" << "$" << "%" << "^" << "&" << "*" << "(" << ")" << "_" << "+" << "Q" << "W" << "E" << "R" << "T" << "Y" << "U" << "I" << "O" << "P" << "{" << "}" << "|" << "A" << "S" << "D" << "F" << "G" << "H" << "J" << "K" << "L" << ":" << "\"" << "Z" << "X" << "C" << "V" << "B" << "N" << "M" << "<" << ">" << "?";
    return ret;
}
int AntiKeyAndMouseLogger::numEntriesOnOneKeymapPage()
{
    return m_EntrySelectionKeys.size(); //soon this would be size/2 (for "non-shift" and "shift" pages), but ultimately it could be arbitrary/run-time-specifiable-depending-on-screen-size-etc and this app could cover much more (all?) of the unicode keymap
}
void AntiKeyAndMouseLogger::insertSpecialNextPageKeyIntoRandomPositionOnPage(KeyMap *currentPage)
{
    int indexOfAnExistingEntryToSwapSpecialLastPageButtonWith = qrand() % currentPage->size();
    KeymapHashTypes anExistingEntryToSwapSpecialLastPageButtonWith = currentPage->at(indexOfAnExistingEntryToSwapSpecialLastPageButtonWith);
    currentPage->replace(indexOfAnExistingEntryToSwapSpecialLastPageButtonWith, qMakePair(anExistingEntryToSwapSpecialLastPageButtonWith.first, QtKeyToString(AntiKeyAndMouseLogger_NEXT_PAGE_SPECIAL_SYMBOL)));
    currentPage->append(qMakePair(m_EntrySelectionKeys.at(currentPage->size()), anExistingEntryToSwapSpecialLastPageButtonWith.second));
}
void AntiKeyAndMouseLogger::generateShuffledKeymapAndRequestPresentationOfFirstPage()
{
    m_CurrentShuffledKeymapPageIndex = 0;
    m_ShuffledKeymapPages.clear(); //TODOmb: on shutdown/destruct/whatever (or just on clear), I should overwrite the memory. LibCrypo ++has SecMem classes maybe I should use? Oh wait no this app assumes your comp is not compromised duh, otherwise all is [already] lost

    QList<KeyMapEntry> values = m_NonShuffledKeymap;
    KeyMap currentPage;
    while(!values.isEmpty())
    {
        currentPage.append(qMakePair(m_EntrySelectionKeys.at(currentPage.size()), values.takeAt(qrand() % values.size())));

        //if there's only one entry slot left on this page, fill it with the special 'next page' button (but we put it in a random spot, swapping it with a different entry)
        if(currentPage.size() == (numEntriesOnOneKeymapPage()-1))
        {
            insertSpecialNextPageKeyIntoRandomPositionOnPage(&currentPage); //the swap

            //we also know now that this page is now full, so we prepare for filling up the next one
            m_ShuffledKeymapPages.append(currentPage);
            currentPage.clear();
        }
    }
    if(!currentPage.isEmpty())
    {
        insertSpecialNextPageKeyIntoRandomPositionOnPage(&currentPage);
        m_ShuffledKeymapPages.append(currentPage);
    }

    m_CurrentShuffledKeymapPage = m_ShuffledKeymapPages.first();
    emit presentShuffledKeymapPageRequested(m_CurrentShuffledKeymapPage);
}
void AntiKeyAndMouseLogger::translateShuffledKeymapEntry(const QString &shuffledKeymapEntry)
{
    Q_FOREACH(const KeymapHashTypes &currentEntry, m_CurrentShuffledKeymapPage)
    {
        if(shuffledKeymapEntry == currentEntry.first)
        {
            if(currentEntry.second == QtKeyToString(AntiKeyAndMouseLogger_NEXT_PAGE_SPECIAL_SYMBOL))
            {
                ++m_CurrentShuffledKeymapPageIndex;
                if(m_CurrentShuffledKeymapPageIndex >= m_ShuffledKeymapPages.size())
                {
                    generateShuffledKeymapAndRequestPresentationOfFirstPage();
                    return;
                }
                m_CurrentShuffledKeymapPage = m_ShuffledKeymapPages.at(m_CurrentShuffledKeymapPageIndex); //TODOmb: mb an iterator would be cleaner than an index
                emit presentShuffledKeymapPageRequested(m_CurrentShuffledKeymapPage);
                return;
            }

            emit shuffledKeymapEntryTranslated(currentEntry.second);
            generateShuffledKeymapAndRequestPresentationOfFirstPage();
            return;
        }
    }
    //TODOmb: error checking? if the frontend gives us a key not in the map, we'd become unusable because of a lack of a signal response here
}
