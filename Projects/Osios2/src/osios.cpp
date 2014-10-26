#include "osios.h"

#include <QSettings>
#include <QTimer>
#include <QFile>
#include <QDataStream>
#include <QDebug>

#include "osioscommon.h"
#include "timelineserializer.h"

#define OSIOS_TIMELINE_FILENAME "timeline.bin"

//TODOoptional: COW on the timeline nodes (the seraialized bytes, that is) could be achieved with a timeline node called "import state from doc x, mutate with empty character" (or perhaps these two are separated), effectively incrementing the reference count (although delete is not in plans anyways)
Osios::Osios(const QString &profileName, QObject *parent)
    : QObject(parent)
    , m_ProfileName(profileName)
    , m_LastSerializedTimelineIndex(-1)
{
    QSettings settings;
    settings.beginGroup(PROFILES_GOUP_SETTINGS_KEY);
    settings.beginGroup(profileName);
    QString dataDir = settings.value(OSIOS_DATA_DIR_SETTINGS_KEY).toString(); //TODOoptional: sanitize empty/invalid filename etc. also the value needs to be set during profile creation (and the dir/filename needs to be queried)
    settings.endGroup();
    settings.endGroup();
    m_LocalPersistenceDevice = new QFile(appendSlashIfNeeded(dataDir) + OSIOS_TIMELINE_FILENAME, this);
    if(!m_LocalPersistenceDevice->open(QIODevice::ReadWrite))
    {
        //TODOoptional: better handling
        return;
    }
    m_LocalPersistenceStream.setDevice(m_LocalPersistenceDevice);

    qRegisterMetaType<TimelineNode>("TimelineNode");

    //read in all previously serialized entries, which also puts the write cursor is at the end for appending
    readInAllPreviouslySerializedEntries();

    int timelineSize = m_TimelineNodes.size();
    qDebug() << "Read in " << QString::number(timelineSize) << " previously serialized entries in the timeline";
    if(timelineSize == 0)
    {
        //no timeline nodes, so write zero size placeholder into file, which is rewound to and updated in our destructor
        m_LocalPersistenceStream << timelineSize;
    }

    QTimer *diskFlushTimer = new QTimer(this);
    connect(diskFlushTimer, SIGNAL(timeout()), this, SLOT(flushDiskBuffer()));
    diskFlushTimer->start(2*60*1000); //2 mins
}
Osios::~Osios()
{
    //update the [probably, but not necessarily, changed] serialized number of timeline nodes at the beginning of the file (better, use a mutation append-only strategy)
    m_LocalPersistenceDevice->seek(0);
    m_LocalPersistenceStream << m_TimelineNodes.size();
    m_LocalPersistenceDevice->flush();
    qDeleteAll(m_TimelineNodes);
}
QList<TimelineNode> Osios::timelineNodes() const
{
    return m_TimelineNodes;
}
void Osios::readInAllPreviouslySerializedEntries()
{
    while(!m_LocalPersistenceStream.atEnd())
    {
        int numTimelineNodes;
        //if(m_LocalPersistenceStream.device()->bytesAvailable() < sizeof(numTimelineNodes)) -- not network so yea
        //    return;
        m_LocalPersistenceStream >> numTimelineNodes;
        int currentIndex = m_TimelineNodes.size()-1;
        for(int i = 0; i < numTimelineNodes; ++i)
        {
            ITimelineNode *serializedTimelineNode = deserializeNextTimelineNode();
            //m_LocalPersistenceStream >> serializedTimelineNode;
            m_TimelineNodes.append(serializedTimelineNode);
            ++currentIndex;
            m_LastSerializedTimelineIndex = currentIndex;
        }
    }
}
ITimelineNode *Osios::deserializeNextTimelineNode()
{
    //we have to return a pointer because we have to instantiate the derived type
    return TimelineSerializer::peekInstantiateAndDeserializeNextTimelineNodeFromIoDevice(m_LocalPersistenceDevice);
}
//batch write, but really doesn't make much difference since we flush every X seconds anyways
void Osios::serializeTimelineAdditionsLocally()
{
    int timelineSize = m_TimelineNodes.size();
    while((m_LastSerializedTimelineIndex+1) < timelineSize)
    {
        TimelineNode timelineNode = m_TimelineNodes.at(++m_LastSerializedTimelineIndex);
        serializeTimelineActionLocally(timelineNode);
    }
}
void Osios::serializeTimelineActionLocally(TimelineNode action)
{
    m_LocalPersistenceStream << *action;
}
void Osios::flushDiskBuffer()
{
    if(!m_LocalPersistenceDevice->flush())
    {
        //TODOreq: error notification
    }
    //TODOreq: still need to do "sync" system call, but i don't think there's a portable way to do it :(
}
void Osios::propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(TimelineNode action)
{
    //TODOreq:
}
void Osios::recordAction(TimelineNode action)
{
    m_TimelineNodes.append(action); //takes ownership, deletes in destructor
    //TODOreq: at this point we schedule it to be written to disk in 2 mins (we just give it to a thread (main thread for now, but shouldn't be later) that is periodically flushed), but immediately replicate it to network neighbors for cryptographic verification
    propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(action);
    serializeTimelineAdditionsLocally(); //writes to file but does not flush
}
