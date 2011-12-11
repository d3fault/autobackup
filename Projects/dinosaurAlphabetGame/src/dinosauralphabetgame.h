#ifndef DINOSAURALPHABETGAME_H
#define DINOSAURALPHABETGAME_H

#include <QObject>

class DinosaurAlphabetGame : public QObject
{
    Q_OBJECT
public:
    explicit DinosaurAlphabetGame(QObject *parent = 0);
private:
    int m_NumbersToShowAtATime;
    void myConstructor();
signals:

public slots:
    void start();
    void loopSlot();
    void handleKeyPressed(Qt::Key key);
};

#endif // DINOSAURALPHABETGAME_H
