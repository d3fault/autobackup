/*
Copyright (c) 2012, d3fault <d3faultdotxbe@gmail.com>
Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE
*/
#ifndef OBJECTONTHREADHELPER_H
#define OBJECTONTHREADHELPER_H

#include <QThread>

//This base class is just a hack around MOC not playing nicely with templates
class ObjectOnThreadHelperBase : public QThread
{
    Q_OBJECT
public:
    explicit ObjectOnThreadHelperBase(QObject *parent = 0) : QThread(parent), m_TheObject(0) { }
protected:
    QObject *m_TheObject;
    virtual void run() = 0;
signals:
    void objectIsReadyForConnectionsOnly();
};

template<class UserObjectType>
class ObjectOnThreadHelper : public ObjectOnThreadHelperBase
{
public:
    UserObjectType *getObjectPointerForConnectionsOnly()
    {
        return static_cast<UserObjectType*>(m_TheObject);
    }
protected:
    virtual void run()
    {
        //The user's object is allocated on the stack right when the thread starts
        UserObjectType theObject;

        //We remember it's address in a member pointer so the thread that created us can request it when they respond to our objectIsReadyForConnectionsOnly() signal. Due to limitations with MOC, we cannot emit a templated type
        m_TheObject = &theObject;

        //Tell whoever created us that the object is ready for connections
        emit objectIsReadyForConnectionsOnly();

        //Enter the event loop and do not leave it until QThread::quit() or QThread::terminate() are called
        exec();

        //Just in case they ask for it again (they shouldn't)
        m_TheObject = 0;


        //'theObject' is destroyed when it goes out of scope. Qt can safely handle signals/slots to a non-existing object, so you don't have to worry about disconnect()'ing them
    }
};

#endif // OBJECTONTHREADHELPER_H
