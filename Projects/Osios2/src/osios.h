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
    QList<ITimelineNode> timelineNodes() const;
private:
    QString m_ProfileName;
    QList<ITimelineNode> m_TimelineNodes;
    int m_LastSerializedTimelineIndex;
    QFile *m_LocalPersistenceDevice;
    QDataStream m_LocalPersistenceStream;

    void readInAllPreviouslySerializedEntries();
    void serializeTimelineAdditionsLocally();
    void serializeTimelineActionLocally(const ITimelineNode &action);
    void propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(const ITimelineNode &action);
    inline QString appendSlashIfNeeded(const QString &inputString)
    {
        if(inputString.endsWith("/"))
        {
            return inputString;
        }
        return inputString + "/";
    }
public slots:
    void recordAction(const ITimelineNode &action);
private slots:
    void flushDiskBuffer();
};

#endif // OSIOS_H
