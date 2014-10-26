#include "osios.h"

#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QDebug>

#include "osioscommon.h"

#define OSIOS_SERIALIZED_TIMELINE_FILEPATH_SETTINGS_KEY "serializedTimelineFilePath"

//TODOoptional: COW on the timeline nodes (the seraialized bytes, that is) could be achieved with a timeline node called "import state from doc x, mutate with empty character" (or perhaps these two are separated), effectively incrementing the reference count (although delete is not in plans anyways)
Osios::Osios(const QString &profileName, QObject *parent)
    : QObject(parent)
    , m_ProfileName(profileName)
    , m_LastSerializedTimelineIndex(-1)
{
    QSettings settings;
    settings.beginGroup(PROFILES_GOUP_SETTINGS_KEY);
    QString timelineFilepath = settings.value(OSIOS_SERIALIZED_TIMELINE_FILEPATH_SETTINGS_KEY).toString(); //TODOoptional: sanitize empty/invalid filename etc
    settings.endGroup();
    m_LocalPersistenceDevice = new QFile(timelineFilepath, this);
    if(!m_LocalPersistenceDevice->open(QIODevice::ReadWrite))
    {
        //TODOoptional: better handling
        return;
    }
    m_LocalPersistenceStream.setDevice(m_LocalPersistenceDevice);

    //TODOreq: read in all previously serialized entries, and make sure write cursor is at the end for appending
    readInAllPreviouslySerializedEntries();

    qDebug() << "Read in " << QString::number(m_Timeline.size()) << " previously serialized entries in the timeline";

    QTimer *diskFlushTimer = new QTimer(this);
    connect(diskFlushTimer, SIGNAL(timeout()), this, SLOT(flushDiskBuffer()));
    diskFlushTimer->start(2*60*1000); //2 mins
}
Osios::~Osios()
{
    //TODOreq: update the [probably, but not necessarily, changed] serialized number of timeline nodes at the beginning of the file (better, use a mutation append-only strategy)
}
void Osios::readInAllPreviouslySerializedEntries()
{
    while(!m_LocalPersistenceStream.atEnd())
    {
        int numTimelineNodes;
        //if(m_LocalPersistenceStream.device()->bytesAvailable() < sizeof(numTimelineNodes)) -- not network so yea
        //    return;
        m_LocalPersistenceStream >> numTimelineNodes;
        for(int i = 0; i < numTimelineNodes; ++i)
        {
            ITimelineNode serializedTimelineNode;
            m_LocalPersistenceStream >> serializedTimelineNode;
            m_Timeline.append(serializedTimelineNode);
        }
    }
}
//batch write, but really doesn't make much difference since we flush every X seconds anyways
void Osios::serializeTimelineAdditionsLocally()
{
    int timelineSize = m_Timeline.size();
    while((m_LastSerializedTimelineIndex+1) < timelineSize)
    {
        const ITimelineNode &timelineNode = m_Timeline.at(++m_LastSerializedTimelineIndex);
        serializeTimelineActionLocally(timelineNode);
    }
}
void Osios::serializeTimelineActionLocally(const ITimelineNode &action)
{
    m_LocalPersistenceStream << action;
}
void Osios::flushDiskBuffer()
{
    if(!m_LocalPersistenceDevice->flush())
    {
        //TODOreq: error notification
    }
    //TODOreq: still need to do "sync" system call, but i don't think there's a portable way to do it :(
}
void Osios::propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(const ITimelineNode &action)
{
    //TODOreq:
}
void Osios::recordAction(const ITimelineNode &action)
{
    m_Timeline.append(action);
    //TODOreq: at this point we schedule it to be written to disk in 2 mins (we just give it to a thread (main thread for now, but shouldn't be later) that is periodically flushed), but immediately replicate it to network neighbors for cryptographic verification
    propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(action);
    serializeTimelineAdditionsLocally(); //writes to file but does not flush
}
