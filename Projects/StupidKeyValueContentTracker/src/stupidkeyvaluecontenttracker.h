#ifndef STUPIDKEYVALUECONTENTTRACKER_H
#define STUPIDKEYVALUECONTENTTRACKER_H

#include <QObject>

#include <QSettings>
#include <QJsonObject>

#include "stupidkeyvaluecontenttrackertypes.h"
#include "timeanddata_timeline.h"

#define StupidKeyValueContentTracker_SETTINGSKEY_HAVECACHE "haveCache"
#define StupidKeyValueContentTracker_SETTINGSKEY_CACHEGROUP "cache"

#define StupidKeyValueContentTracker_JSONKEY_COMMITMESSAGE "commitMessage"
#define StupidKeyValueContentTracker_JSONKEY_BULKMUTATIONS "mutations"

class StupidKeyValueContentTracker : public QObject
{
    Q_OBJECT
public:
    template<class T>
    static void establishConnectionsToAndFromBackendAndUi(StupidKeyValueContentTracker *backend, T *ui)
    {
        connect(ui, &T::initializeRequested, backend, &StupidKeyValueContentTracker::initialize);
        connect(ui, &T::addRequested, backend, &StupidKeyValueContentTracker::add);
        connect(ui, &T::removeKeyRequested, backend, &StupidKeyValueContentTracker::removeKey);
        connect(ui, &T::commitRequested, backend, &StupidKeyValueContentTracker::commit);
        connect(ui, &T::readKeyRequested, backend, &StupidKeyValueContentTracker::readKey);
        connect(backend, &StupidKeyValueContentTracker::e, ui, &T::handleE);
        connect(backend, &StupidKeyValueContentTracker::o, ui, &T::handleO);
        connect(backend, &StupidKeyValueContentTracker::initializationFinished, ui, &T::handleInitializationFinished);
        connect(backend, &StupidKeyValueContentTracker::addFinished, ui, &T::handleAddFinished);
        connect(backend, &StupidKeyValueContentTracker::removeKeyFinished, ui, &T::handleRemoveKeyFinished);
        connect(backend, &StupidKeyValueContentTracker::commitFinished, ui, &T::handleCommitFinished);
        connect(backend, &StupidKeyValueContentTracker::readKeyFinished, ui, &T::handleReadKeyFinished);
    }

    explicit StupidKeyValueContentTracker(QObject *parent = 0);
    ~StupidKeyValueContentTracker();
private:
    void populateCurrentDataWithCache(QSettings &settings);
    void commitStagedKeyValueStoreMutations_ThenEmitCommitFinished(const QString &commitMessage);
    void populateCommitDataUsingStagedKeyValueStoreMutations(QJsonObject &bulkMutations);
    void commitActual_ThenEmitCommitFinished(const QJsonObject &commitData, const QString &commitMessage);
    void applyStagedMutationsToCurrentData();

    TimeAndData_Timeline *m_Timeline;
    StagedMutationsType m_StagedKeyValueStoreMutation; //TODOreq: Implicit Sharing instead of QSharedPointer? implicit sharing is similar but safer/better (takes longer to set up tho)!!
    CurrentDataType m_CurrentData;
signals:
    void e(const QString &msg);
    void o(const QString &msg);

    void initializationFinished(bool success);
    void addFinished(bool success);
    void removeKeyFinished(bool success);
    void commitFinished(bool success);
    void readKeyFinished(bool success, const QString &key, const QString &revision, const QString &data);

    //PRIVATE SIGNALS:
    void retrieveAllTimelineEntriesRequested();
    void appendJsonObjectToTimelineRequested(const QJsonObject &commitData);
public slots:
    void initialize();
    void add(const QString &key, const QString &data);
    void removeKey(const QString &key);
    void commit(const QString &commitMessage);
    void readKey(const QString &key, const QString &revision);
private slots:
    void handleAllTimelineEntriesRead(const AllTimelineEntriesType &allTimelineEntries);
    void handleFinishedAppendingJsonObjectToTimeline_aka_emitCommitFinished(bool success);
};

#endif // STUPIDKEYVALUECONTENTTRACKER_H
