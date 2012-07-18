#ifndef IDEBUGGABLESTARTABLESTOPPABLEBANKSERVERBACKEND_H
#define IDEBUGGABLESTARTABLESTOPPABLEBANKSERVERBACKEND_H

#include <QObject>

class IDebuggableStartableStoppableBankServerBackend : public QObject
{
    Q_OBJECT
signals:
    void initialized();
    void started();
    void stopped();
    void d(const QString &);
public slots:
    virtual void init()=0;
    virtual void start()=0;
    virtual void stop()=0;
};

#endif // IDEBUGGABLESTARTABLESTOPPABLEBANKSERVERBACKEND_H
