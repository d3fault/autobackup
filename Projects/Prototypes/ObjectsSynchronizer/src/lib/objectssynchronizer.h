#ifndef OBJECTSSYNCHRONIZER_H
#define OBJECTSSYNCHRONIZER_H

#include <QObject>
#include <QHash>
#include <QList>

#include "objectonthreadhelper.h"

class ObjectIsReadyForConnectionsOnlyRelayer : public QObject //holy shit where have you been all my life
{
    Q_OBJECT
public:
    explicit ObjectIsReadyForConnectionsOnlyRelayer(const QString &startedCallbackSlotToRelayObjectIsReadyForConnectionsTo, ObjectOnThreadHelperBase *objectOnThreadHelper, QObject *parent = 0) : QObject(parent), m_StartedCallbackSlotToRelayObjectIsReadyForConnectionsTo(startedCallbackSlotToRelayObjectIsReadyForConnectionsTo), m_ObjectOnThreadHelperBase(objectOnThreadHelper) { }
    static QObject *m_SynchronizerNotifiee;
private:
    const QString &m_StartedCallbackSlotToRelayObjectIsReadyForConnectionsTo;
    ObjectOnThreadHelperBase *m_ObjectOnThreadHelperBase;
public slots:
    void handleObjectIsReadyForConnections()
    {
        QMetaObject::invokeMethod(m_SynchronizerNotifiee, m_StartedCallbackSlotToRelayObjectIsReadyForConnectionsTo.toStdString().c_str(), Q_ARG(QObject*, m_ObjectOnThreadHelperBase->getBaseObjectPointerForConnectionsOnly()));
        deleteLater();
    }
};

class ObjectsSynchronizer : public QObject
{
    Q_OBJECT
public:
    explicit ObjectsSynchronizer(QObject *synchronizerNotifiee, bool cancelAllWhenOneFails = true, QObject *parent = 0);
    template<class T>
    void addAndConstructAndInitializeAndStartObject(const QString &startedCallbackSlot_AKA_object_ready_for_connections, bool onSeparateThread = false);
private:
    friend class ObjectIsReadyForConnectionsOnlyRelayer;

    QObject *m_SynchronizerNotifiee;
    bool m_CancelAllWhenOneFails;
    QHash<QObject*, int> m_ObjectsHash;
    QList<ObjectOnThreadHelperBase*> m_ThreadGroupsHash;
signals:
};

//bah template methods can't go in source? there's gotta be a way...
template<class T>
void ObjectsSynchronizer::addAndConstructAndInitializeAndStartObject(const QString &startedCallbackSlot_AKA_object_ready_for_connections, bool onSeparateThread)
{
    if(!onSeparateThread)
    {
        T *t = new T();
        m_ObjectsHash.insert(static_cast<QObject*>(t), -1);
        QMetaObject::invokeMethod(m_SynchronizerNotifiee, startedCallbackSlot_AKA_object_ready_for_connections.toStdString().c_str(), Q_ARG(QObject*, static_cast<QObject*>(t)));
        return;
    }

    ObjectOnThreadHelper<T> *objectOnThreadHelper = new ObjectOnThreadHelper<T>(this);
    ObjectIsReadyForConnectionsOnlyRelayer *tempRelayer = new ObjectIsReadyForConnectionsOnlyRelayer(startedCallbackSlot_AKA_object_ready_for_connections, objectOnThreadHelper);
    connect(objectOnThreadHelper, SIGNAL(objectIsReadyForConnectionsOnly()), tempRelayer, SLOT(handleObjectIsReadyForConnections()));
    //later, insert into object hash
}


#endif // OBJECTSSYNCHRONIZER_H
