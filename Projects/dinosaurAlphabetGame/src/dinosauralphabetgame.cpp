#include "dinosauralphabetgame.h"

DinosaurAlphabetGame::DinosaurAlphabetGame(QObject *parent) :
    QObject(parent)
{
}
void DinosaurAlphabetGame::myConstructor()
{
    m_CurrentKeySet = new QMap<int,Qt::Key>();
    m_MissedKeys = new QList<Qt::Key>();
    m_NumbersToShowAtATime = 5;
    AlphabetSize = ((Qt::Key_Z - Qt::Key_A) + 1); //off by one fix
}
void DinosaurAlphabetGame::start()
{
    //maybe i'm just paranoid, but i don't trust anything created in my real constructor because at the time any object in there is created it is before we have called "moveToThread" from the widget (which is on the gui thread at the time)
    myConstructor();

    getKeys();
}
void DinosaurAlphabetGame::handleKeyPressed(Qt::Key key)
{
    if(isKeyWeWant(key))
    {
        if(keyWeAreProcessingIsLastKeyInSet())
        {
            getKeysOrRecycle(); //get new keys, OR if any are marked as being ones we got wrong at least once (didn't get it right on first try)
        }
        else
        {
            //append key index
            ++m_CurrentKeyIndex;
            //get key using new index from key set
            m_CurrentKey = m_CurrentKeySet->value(m_CurrentKeyIndex);
        }
    }
    else
    {
        m_MissedKeys->append(key);
    }
}
void DinosaurAlphabetGame::getKeys()
{
    seedRandom();
    m_CurrentKeySet->clear();
    for(int i = 0; i < m_NumbersToShowAtATime; ++i)
    {
        //get a random number between 0 and AlphabetSize
        int randomNumber = (qrand() % AlphabetSize);

        Qt::Key newKey = (Qt::Key)(Qt::Key_A + randomNumber); //todo if you wanted to expand the keyset, use m_StartOfKeyRange (assigned to Qt::Key_A in this example) here instead. can also make it easier to support a different language. they specify the range and the start of key range is deduced from that
        m_CurrentKeySet->insert(i, newKey);
        //TODOreq: make sure we don't have the same letter in the set twice
    }
    setCurrentKeyToFirstKeyInSet();
}
void DinosaurAlphabetGame::getKeysOrRecycle()
{
    if(m_MissedKeys->size() > 0)
    {
        recycleMissedKeys();
    }
    else
    {
        getKeys();
    }
    m_MissedKeys->clear();
}
bool DinosaurAlphabetGame::isKeyWeWant(Qt::Key key)
{
    return (m_CurrentKey == key);
}
bool DinosaurAlphabetGame::keyWeAreProcessingIsLastKeyInSet()
{
    return (m_CurrentKeyIndex == (m_NumbersToShowAtATime - 1));
}
void DinosaurAlphabetGame::recycleMissedKeys()
{
    seedRandom();
    Qt::Key lastKey;
    lastKey = m_CurrentKeySet->values().last(); //for making sure we don't do the same letter twice
    m_CurrentKeySet->clear();
    int missedKeysCount = m_MissedKeys->count();
    int missedKeysLeft = missedKeysCount;
    for(int i = 0; i < missedKeysCount; ++i)
    {
        int missedKeyIndex = (qrand() % missedKeysLeft);
        Qt::Key missedKey;
        missedKey = m_MissedKeys->at(missedKeyIndex);
        //i == 0: only compare first in new set
        //missedKey == last key: make sure our first key is not our last key from last set (same key twice in a row is a no no)
        //missedKeysCount > 1: ignore all this if we only missed one key. we don't care in that one rare case if we do the same key twice
        while(i == 0 && missedKey == lastKey && missedKeysCount > 1)
        {
            seedRandom();
            missedKeyIndex = (qrand() % missedKeysLeft);
            missedKey = m_MissedKeys->at(missedKeyIndex);
        }
        m_CurrentKeySet->insert(i, missedKey);
        m_MissedKeys->removeAt(missedKeyIndex);
        --missedKeysLeft;
    }
    setCurrentKeyToFirstKeyInSet();
}
void DinosaurAlphabetGame::seedRandom()
{
    qsrand(QTime::currentTime().hour() + QTime::currentTime().minute() + QTime::currentTime().second() + QTime::currentTime().msec());
}
void DinosaurAlphabetGame::setCurrentKeyToFirstKeyInSet()
{
    m_CurrentKeyIndex = 0;
    m_CurrentKey = m_CurrentKeySet->value(m_CurrentKeyIndex);
}
