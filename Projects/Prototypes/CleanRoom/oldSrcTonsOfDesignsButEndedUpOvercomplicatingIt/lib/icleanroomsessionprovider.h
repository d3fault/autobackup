#ifndef ICLEANROOMSESSIONPROVIDER
#define ICLEANROOMSESSIONPROVIDER

#include <QMetaMethod>

#include "icleanroomsession.h"

class QObject;

class ICleanRoomSessionProvider
{
public:
    //void setMetaMethodToCallWhenNewSession(QObject *metaObject, QMetaMethod metaMethod)
    ICleanRoomSessionProvider(QObject *metaObjectToCallWhenNewSession, QMetaMethod metaMethodToCallWhenNewSession)
        : m_MetaObjectToCallWhenNewSession(metaObjectToCallWhenNewSession)
        , m_MetaMethodToCallWhenNewSession(metaMethodToCallWhenNewSession)
    { }
protected:
    QObject *m_MetaObjectToCallWhenNewSession;
    QMetaMethod m_MetaMethodToCallWhenNewSession;
    void handleNewSession(ICleanRoomSession *cleanRoomSession)
    {
        m_MetaMethodToCallWhenNewSession.invoke(m_MetaObjectToCallWhenNewSession, Qt::AutoConnection, Q_ARG(ICleanRoomSession*, cleanRoomSession));
    }
};

#endif // ICLEANROOMSESSIONPROVIDER

