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
private:
    QString m_ProfileName;
    QList<ITimelineNode> m_Timeline;
    int m_LastSerializedTimelineIndex;
    QFile *m_LocalPersistenceDevice;
    QDataStream m_LocalPersistenceStream;

    void readInAllPreviouslySerializedEntries();
    void serializeTimelineAdditionsLocally();
    void serializeTimelineActionLocally(const ITimelineNode &action);
    void flushDiskBuffer();
    void propagateActionToNeighborsAsFirstStepOfCryptographicallyVerifyingItsReplication(const ITimelineNode &action);
public slots:
    void recordAction(const ITimelineNode &action);
};

#endif // OSIOS_H
