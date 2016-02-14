#ifndef ANTIKEYANDMOUSELOGGER_H
#define ANTIKEYANDMOUSELOGGER_H

#include <QObject>

#include <QHash>
#include <QKeySequence>

typedef int KeyMapEntry; //Qt::Key_
#define KeymapHashTypes KeyMapEntry /*what they type*/, KeyMapEntry /*what it translates to*/
typedef QHash<KeymapHashTypes> KeyMap;
typedef QHashIterator<KeymapHashTypes> KeyMapIterator;

class AntiKeyAndMouseLogger : public QObject
{
    Q_OBJECT
public:
    explicit AntiKeyAndMouseLogger(QObject *parent = 0);
    static QList<KeyMapEntry> staticInitNonShuffledKeymap();
    static int numEntriesOnOneKeymapPage();
    static inline QString keymapEntryToString(const KeyMapEntry &keyMapEntry) { return QKeySequence(keyMapEntry).toString().toLower(); }
private:
    KeyMap m_ShuffledKeymap;

    static QList<KeyMapEntry> m_NonShuffledKeymap;
signals:
    void shuffledKeymapGenerated(const KeyMap &shuffledKeymap);
    void shuffledKeymapEntryTranslated(const KeyMapEntry &translatedKey);
public slots:
    void generateShuffledKeymap();
    void translateShuffledKeymapEntry(const int &shuffledKeymapEntry);
};

#endif // ANTIKEYANDMOUSELOGGER_H
