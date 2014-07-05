#ifndef FOO_H
#define FOO_H

#include <QObject>

#include "objectonthreadgroup.h"

class Bar;

class Foo : public QObject
{
    Q_OBJECT
public:
    explicit Foo(QObject *parent = 0);
private:
    ObjectOnThreadGroup *m_BarThread;
    Bar *m_Bar;
signals:
    void allChildrenOnDifferentThreadsInitialized();
    void fooSignal(bool success);
public slots:
    void fooSlot(const QString &cunt);
private slots:
    void handleBarReadyForConnections(QObject *barAsQObject);
    void handleBarSignal(bool success);
};

#endif // FOO_H
