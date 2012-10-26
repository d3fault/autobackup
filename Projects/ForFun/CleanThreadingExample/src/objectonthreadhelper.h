#ifndef OBJECTONTHREADHELPER_H
#define OBJECTONTHREADHELPER_H

#include <QThread>


#ifdef ORIG_ATTEMPT
template <class UserObjectInTemplate>
class ObjectOnThreadHelperTemplateHack : public QThread
{
    //Q_OBJECT
public:
    explicit ObjectOnThreadHelperTemplateHack(QObject *parent = 0) : QThread(parent), m_PointerToObject(0) { }
    UserObjectInTemplate *getPointerToObjectOnThreadToBeUsedOnlyForConnections() //dirty hack because template types can't be signal parameters :(
    {
        return m_PointerToObject;
    }
private:
    UserObjectInTemplate *m_PointerToObject;
protected:
    virtual void run() { }
//signals:
    virtual void objectIsReadyForConnections() = 0;
};

class ObjectOnThreadHelper : public ObjectOnThreadHelperTemplateHack<UserObject>
{
    Q_OBJECT
public:
    ObjectOnThreadHelper(QObject *parent = 0) : ObjectOnThreadHelperTemplateHack<UserObject>(parent) { }
protected:
    virtual void run()
    {
        UserObject userObject;
        m_PointerToObject = &userObject;
        emit objectIsReadyForConnections();
        exec(); //We stay in this event loop until QThread::quit() is called
        m_PointerToObject = 0; //just in case for some reason they ask for it again :-/
    }
signals:
    void objectIsReadyForConnections();
};

template <class UserObject>
class ObjectOnThreadHelper : public QThread
{
    Q_OBJECT
public:
    explicit ObjectOnThreadHelper(QObject *parent = 0) : QThread(parent), m_PointerToObject(0) { }
    UserObject *getPointerToObjectOnThreadToBeUsedOnlyForConnections() //dirty hack because template types can't be signal parameters :(
    {
        return m_PointerToObject;
    }
private:
    UserObject *m_PointerToObject;
protected:
    virtual void run()
    {
        UserObject userObject;
        m_PointerToObject = &userObject;
        emit objectIsReadyForConnections();
        exec(); //We stay in this event loop until QThread::quit() is called
        m_PointerToObject = 0; //just in case for some reason they ask for it again :-/
    }
signals:
    void objectIsReadyForConnections();
};
#endif

#endif // OBJECTONTHREADHELPER_H
