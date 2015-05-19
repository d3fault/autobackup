#ifndef ICLEANROOMSESSIONPROVIDER
#define ICLEANROOMSESSIONPROVIDER

#include <QMetaMethod>

#include "cleanroomsession.h"

class QObject;

class ICleanRoomSessionProvider
{
public:
    //void setMetaMethodToCallWhenNewSession(QObject *metaObject, QMetaMethod metaMethod)
    ICleanRoomSessionProvider(QObject *metaObject, QMetaMethod metaMethod)
        : m_MetaObjectToCallWhenNewSession(metaObject)
        , m_MetaMethodToCallWhenNewSession(metaMethod)
    { }
protected:
    QObject *m_MetaObjectToCallWhenNewSession;
    QMetaMethod m_MetaMethodToCallWhenNewSession;

    void handleNewSession(CleanRoomSession cleanRoomSession)
    {
        m_MetaMethodToCallWhenNewSession.invoke(m_MetaObjectToCallWhenNewSession, Qt::QueuedConnection, Q_ARG(CleanRoomSession, cleanRoomSession));
    }
};

#endif // ICLEANROOMSESSIONPROVIDER

