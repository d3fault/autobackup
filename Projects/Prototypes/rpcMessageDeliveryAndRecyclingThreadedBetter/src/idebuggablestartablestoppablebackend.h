#ifndef IDEBUGGABLESTARTABLESTOPPABLEBACKEND_H
#define IDEBUGGABLESTARTABLESTOPPABLEBACKEND_H

#include <QObject>

class IDebuggableStartableStoppableBackend : public QObject
{
    Q_OBJECT
signals:
    void d(const QString &);
    void initialized();
    void started();
    void stopped();
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;
    virtual void simulateCreateBankAccount()=0;
};

#endif // IDEBUGGABLESTARTABLESTOPPABLEBACKEND_H
