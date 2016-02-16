#include "antikeyandmouselogger.h"

QList<KeyMapEntry> AntiKeyAndMouseLogger::m_EntrySelectionKeys = allTypeableOnUsKeyboardWithoutNeedingShiftKey();
QList<KeyMapEntry> AntiKeyAndMouseLogger::m_NonShuffledKeymap = allTypeableKeysOnUsKeyboard();

//TODOreq: if I implement "clear clipboard after 30 seconds", I should make sure that what I'm clearing is [still] there password. We don't want to clear it if it isn't. I wonder if KeePass does this check O_o (could find out right now but fuck it), I should file a bug if not
//TODOreq: my instincts told me not to use 'spacebar' as an 'input' key, because how would I represent it without taking up more than one character (another entry in yee ole' legend?)? But after thinking about it further, space is a perfectly valid 'output' (used in passwords) key and so I have the same problem on both sides. Representing it like this: " " (as in, showing the quotes on the button) is no good because it takes up 3 characters instead of 1 :(, which might leak tiny bits of entropy that add up ove time ;-P. Well making it a legend/special key works but only for the output side of things, aww well I guess it's good enough really
AntiKeyAndMouseLogger::AntiKeyAndMouseLogger(QObject *parent)
    : QObject(parent)
    , m_Rng(true) //TODOreq: see notes in initial.idea.txt about not using keyboard/mouse entropy
    , m_CurrentShuffledKeymapPageIndex(0)
{ }
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
    ret << QtKeyToString(AntiKeyAndMouseLogger_SPACE_BAR_SPECIALSYMBOL); //yes, space bar is pressable without the shift key, but there's no way to represent it without taking up more than one character (or, it'd be too difficult to type some funky unicode (via alt+69420) to represent it. so space is an OUTPUT key only)
    return ret;
}
bool AntiKeyAndMouseLogger::isTypeableOnUsKeyboardWithoutNeedingShiftKey(const QString &key)
{
    return m_EntrySelectionKeys.contains(key);
}
bool AntiKeyAndMouseLogger::isTypeableOnUsKeyboard(const QString &key)
{
    return m_NonShuffledKeymap.contains(key);
}
int AntiKeyAndMouseLogger::numEntriesOnOneKeymapPage()
{
    return m_EntrySelectionKeys.size(); //soon this would be size/2 (for "non-shift" and "shift" pages), but ultimately it could be arbitrary/run-time-specifiable-depending-on-screen-size-etc and this app could cover much more (all?) of the unicode keymap
}
void AntiKeyAndMouseLogger::insertSpecialNextPageKeyIntoRandomPositionOnPage(KeyMap *currentPage)
{
    int indexOfAnExistingEntryToSwapSpecialLastPageButtonWith = m_Rng.GenerateWord32(0, currentPage->size()-1);
    KeymapHashTypes anExistingEntryToSwapSpecialLastPageButtonWith = currentPage->at(indexOfAnExistingEntryToSwapSpecialLastPageButtonWith);
    currentPage->replace(indexOfAnExistingEntryToSwapSpecialLastPageButtonWith, qMakePair(anExistingEntryToSwapSpecialLastPageButtonWith.first, QtKeyToString(AntiKeyAndMouseLogger_NEXT_PAGE_SPECIAL_SYMBOL)));
    currentPage->append(qMakePair(m_EntrySelectionKeys.at(currentPage->size()), anExistingEntryToSwapSpecialLastPageButtonWith.second));
}
void AntiKeyAndMouseLogger::generateShuffledKeymapAndRequestPresentationOfFirstPage()
{
    m_CurrentShuffledKeymapPageIndex = 0;
    m_ShuffledKeymapPages.clear(); //TODOmb: on shutdown/destruct/whatever (or just on clear), I should overwrite the memory. LibCrypo ++has SecMem classes maybe I should use? Oh wait no this app assumes your comp is not compromised duh, otherwise all is [already] lost

    QList<KeyMapEntry> values = m_NonShuffledKeymap;
    m_Rng.Shuffle(values.begin(), values.end());
    KeyMap currentPage;
    while(!values.isEmpty())
    {
        currentPage.append(qMakePair(m_EntrySelectionKeys.at(currentPage.size()), values.takeFirst()));

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
            QString translatedKeyEntry = currentEntry.second;
            if(translatedKeyEntry == QtKeyToString(AntiKeyAndMouseLogger_SPACE_BAR_SPECIALSYMBOL))
                translatedKeyEntry = " ";

            emit shuffledKeymapEntryTranslated(translatedKeyEntry);
            generateShuffledKeymapAndRequestPresentationOfFirstPage();
            return;
        }
    }
    //TO DOnereq: error checking. if the frontend gives us a key not in the map, we'd become unusable because of a lack of a signal response here. this isn't likely when the mouse is used to click buttons, but when the keyboard is used it can happen easily and under normal circumstances: ex alt+10 (maximize), alt+f4 (close). both of those make the GUI blank and the app no longer functional
}
