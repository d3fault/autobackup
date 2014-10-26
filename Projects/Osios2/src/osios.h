#ifndef OSIOS_H
#define OSIOS_H

#include <QObject>

#include "itimelinenode.h"

class QFile;

class Osios : public QObject
{
    Q_OBJECT
public:
    explicit Osios(const QString &profileName, QObject *parent = 0);
    ~Osios();
    QList<TimelineNode> timelineNodes() const;
private:
    friend class TimelineSerializer;

    QString m_ProfileName;
    QList<TimelineNode> m_TimelineNodes;
    int m_LastSerializedTimelineIndex;
    QFile *m_LocalPersistenceDevice;
    QDataStream m_LocalPersistenceStream;

    void readInAllPreviouslySerializedEntries();
    ITimelineNode *deserializeNextTimelineNode();
    void serializeTimelineAdditionsLocally();
    void serializeTimelineActionLocally(TimelineNode action);
    void propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(TimelineNode action);
    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
        {
            return inputString;
        }
        return inputString + "/";
    }
public slots:
    void recordAction(TimelineNode action);
private slots:
    void flushDiskBuffer();
};

#endif // OSIOS_H
