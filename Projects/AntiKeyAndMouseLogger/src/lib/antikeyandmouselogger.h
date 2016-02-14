#ifndef ANTIKEYANDMOUSELOGGER_H
#define ANTIKEYANDMOUSELOGGER_H

#include <QObject>

#include <QHash>
#include <QKeySequence>

typedef QString KeyMapEntry;
#define KeymapHashTypes QPair<KeyMapEntry /*what they type*/, KeyMapEntry> /*what it translates to*/
typedef QList<KeymapHashTypes> KeyMap;
typedef QListIterator<KeymapHashTypes> KeyMapIterator;

class AntiKeyAndMouseLogger : public QObject
{
    Q_OBJECT
public:
    explicit AntiKeyAndMouseLogger(QObject *parent = 0);
    static QList<KeyMapEntry> allTypeableOnUsKeyboardWithoutNeedingShiftKey();
    static QList<KeyMapEntry> allTypeableKeysOnUsKeyboard();
    static int numEntriesOnOneKeymapPage();
    static inline QString QtKeyToString(const int qtKey) { return QKeySequence(qtKey).toString().toLower(); }
private:
    KeyMap m_ShuffledKeymap;

    static QList<KeyMapEntry> m_EntrySelectionKeys;
    static QList<KeyMapEntry> m_NonShuffledKeymap;
signals:
    void presentShuffledKeymapPageRequested(const KeyMap &shuffledKeymapPage);
    void shuffledKeymapEntryTranslated(const KeyMapEntry &translatedKey);
public slots:
    void generateShuffledKeymapAndRequestPresentationOfFirstPage();
    void translateShuffledKeymapEntry(const QString &shuffledKeymapEntry);
};

#endif // ANTIKEYANDMOUSELOGGER_H
