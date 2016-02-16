#ifndef ANTIKEYANDMOUSELOGGER_H
#define ANTIKEYANDMOUSELOGGER_H

#include <QObject>

#include <QPair>
#include <QKeySequence>

#include <crypto++/osrng.h>

typedef QString KeyMapEntry;
#define KeymapHashTypes QPair<KeyMapEntry /*what they type*/, KeyMapEntry> /*what it translates to*/
typedef QList<KeymapHashTypes> KeyMap;
typedef QListIterator<KeymapHashTypes> KeyMapIterator;

#define AntiKeyAndMouseLogger_COLUMNS_PER_ROW 6
#define AntiKeyAndMouseLogger_NEXT_PAGE_SPECIAL_SYMBOL Qt::Key_copyright
#define AntiKeyAndMouseLogger_SPACE_BAR_SPECIALSYMBOL Qt::Key_registered

class AntiKeyAndMouseLogger : public QObject
{
    Q_OBJECT
public:
    explicit AntiKeyAndMouseLogger(QObject *parent = 0);
    static bool isTypeableOnUsKeyboardWithoutNeedingShiftKey(const QString &key);
    static bool isTypeableOnUsKeyboard(const QString &key);
    static int numEntriesOnOneKeymapPage();
    static inline QString QtKeyToString(const int qtKey) { return QKeySequence(qtKey).toString().toLower(); }
private:
    CryptoPP::DefaultAutoSeededRNG m_Rng;
    QList<KeyMap> m_ShuffledKeymapPages;
    int m_CurrentShuffledKeymapPageIndex;
    KeyMap m_CurrentShuffledKeymapPage;

    static QList<KeyMapEntry> m_EntrySelectionKeys;
    static QList<KeyMapEntry> m_NonShuffledKeymap;

    void insertSpecialNextPageKeyIntoRandomPositionOnPage(KeyMap *currentPage);

    static QList<KeyMapEntry> allTypeableOnUsKeyboardWithoutNeedingShiftKey();
    static QList<KeyMapEntry> allTypeableKeysOnUsKeyboard();
signals:
    void presentShuffledKeymapPageRequested(const KeyMap &shuffledKeymapPage);
    void shuffledKeymapEntryTranslated(const KeyMapEntry &translatedKey);
public slots:
    void generateShuffledKeymapAndRequestPresentationOfFirstPage();
    void translateShuffledKeymapEntry(const QString &shuffledKeymapEntry);
};

#endif // ANTIKEYANDMOUSELOGGER_H
