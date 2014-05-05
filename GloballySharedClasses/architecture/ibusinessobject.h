#ifndef IBUSINESSOBJECT_H
#define IBUSINESSOBJECT_H

#include <QObject>

//Abstract interface to be used with or without ObjectOnThreadHelper
class IBusinessObject : public QObject
{
    Q_OBJECT
public:
    explicit IBusinessObject(QObject *parent = 0);
signals:
    void o(const QString &);
    void e(const QString &);
    void initialiedAndStarted();
    void stoppedAndDeinitialized();
    void quitRequested(); //my solution to backends should not call qApp::quit. the owner of the object can decide whether or not it's connected to quit
public slots:
    //void initializeAndStart()=0; //not here because each one needs specialized arguments (and to make readability better, an object specific string appended, ex: initializeAndStartListeningForConnections(QList<QAddress> listOfAddressesToListenToEtc) )
    void stopAndDeinitialize()=0;
};

#endif // IBUSINESSOBJECT_H
