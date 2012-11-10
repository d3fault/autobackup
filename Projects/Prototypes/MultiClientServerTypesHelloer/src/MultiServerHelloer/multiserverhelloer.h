#ifndef MULTISERVERHELLOER_H
#define MULTISERVERHELLOER_H

#include <QObject>

class MultiServerHelloer : public QObject
{
    Q_OBJECT
public:
    explicit MultiServerHelloer(QObject *parent = 0);
    void startAll3Listening();
};

#endif // MULTISERVERHELLOER_H
