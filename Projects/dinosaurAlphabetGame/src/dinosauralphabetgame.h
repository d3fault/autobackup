#ifndef DINOSAURALPHABETGAME_H
#define DINOSAURALPHABETGAME_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QTime>

class DinosaurAlphabetGame : public QObject
{
    Q_OBJECT
public:
    explicit DinosaurAlphabetGame(QObject *parent = 0);
private:
    int m_NumbersToShowAtATime;
    void myConstructor();
    int AlphabetSize;
    QMap<int,Qt::Key> *m_CurrentKeySet;
    QList<Qt::Key> *m_MissedKeys;
    Qt::Key m_CurrentKey;
    int m_CurrentKeyIndex;
    void seedRandom();
    void getKeys();
    void getKeysOrRecycle();
    void recycleMissedKeys();
    bool isKeyWeWant(Qt::Key key);
    bool keyWeAreProcessingIsLastKeyInSet();
signals:
    void keySetChanged(QList<Qt::Key> newKeySet);
    void currentIndexChanged(int newIndex);
public slots:
    void start();
    void handleKeyPressed(Qt::Key key);
};

#endif // DINOSAURALPHABETGAME_H
