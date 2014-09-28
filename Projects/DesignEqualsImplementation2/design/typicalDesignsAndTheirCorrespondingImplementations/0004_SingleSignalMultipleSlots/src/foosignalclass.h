#ifndef FOOSIGNALCLASS_H
#define FOOSIGNALCLASS_H

#include <QObject>

class FooSignalClass : public QObject
{
    Q_OBJECT
public:
    explicit FooSignalClass(QObject *parent = 0);
signals:
    void fooSignal();
public slots:
    void fooSlot();
};

#endif // FOOSIGNALCLASS_H
