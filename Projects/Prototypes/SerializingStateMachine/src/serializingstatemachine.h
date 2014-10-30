#ifndef SERIALIZINGSTATEMACHINE_H
#define SERIALIZINGSTATEMACHINE_H

#include <QObject>

class SerializingStateMachine : public QObject
{
    Q_OBJECT
public:
    explicit SerializingStateMachine(QObject *parent = 0);
};

#endif // SERIALIZINGSTATEMACHINE_H
