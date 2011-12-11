#ifndef DINOSAURALPHABETGAME_H
#define DINOSAURALPHABETGAME_H

#include <QObject>

class DinosaurAlphabetGame : public QObject
{
    Q_OBJECT
public:
    explicit DinosaurAlphabetGame(QObject *parent = 0);

signals:

public slots:
    void start();

};

#endif // DINOSAURALPHABETGAME_H
