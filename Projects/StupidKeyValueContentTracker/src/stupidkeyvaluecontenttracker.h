#ifndef STUPIDKEYVALUECONTENTTRACKER_H
#define STUPIDKEYVALUECONTENTTRACKER_H

#include <QObject>

#include <QHash>
#include <QSharedPointer>
#include <QJsonObject>

#define StupidKeyValueContentTracker_JSONKEY_COMMITMESSAGE "commitMessage"
#define StupidKeyValueContentTracker_JSONKEY_BULKMUTATIONS "mutations"

class IKeyValueStoreMutation;

typedef QSharedPointer<IKeyValueStoreMutation> StupidKeyValueContentTracker_StagedMutationsValueType;

class TimeAndData_Timeline;

class StupidKeyValueContentTracker : public QObject
{
    Q_OBJECT
public:
    template<class T>
    static void establishConnectionsToAndFromBackendAndUi(StupidKeyValueContentTracker *backend, T *ui)
    {
        connect(ui, &T::addRequested, backend, &StupidKeyValueContentTracker::add);
        connect(ui, &T::commitRequested, backend, &StupidKeyValueContentTracker::commit);
        connect(ui, &T::readKeyRequested, backend, &StupidKeyValueContentTracker::readKey);
        connect(backend, &StupidKeyValueContentTracker::e, ui, &T::handleE);
        connect(backend, &StupidKeyValueContentTracker::o, ui, &T::handleO);
        connect(backend, &StupidKeyValueContentTracker::addFinished, ui, &T::handleAddFinished);
        connect(backend, &StupidKeyValueContentTracker::commitFinished, ui, &T::handleCommitFinished);
        connect(backend, &StupidKeyValueContentTracker::readKeyFinished, ui, &T::handleReadKeyFinished);
    }

    explicit StupidKeyValueContentTracker(QObject *parent = 0);
private:
    void commitStagedKeyValueStoreMutations_ThenEmitCommitFinished(const QString &commitMessage);
    void populateCommitDataUsingStagedKeyValueStoreMutations(QJsonObject &bulkMutations);
    void commitActual_ThenEmitCommitFinished(const QJsonObject &commitData, const QString &commitMessage);

    TimeAndData_Timeline *m_Timeline;
    QHash<QString /*key*/, StupidKeyValueContentTracker_StagedMutationsValueType /*value*/> m_StagedKeyValueStoreMutation; //TODOreq: Implicit Sharing instead of QSharedPointer? implicit sharing is similar but safer/better (takes longer to set up tho)!!
signals:
    void e(const QString &msg);
    void o(const QString &msg);

    void addFinished(bool success);
    void commitFinished(bool success);
    void readKeyFinished(bool success, const QString &key, const QString &revision, const QString &data);

    //PRIVATE SIGNALS:
    void appendJsonObjectToTimelineRequested(const QJsonObject &commitData);
public slots:
    void add(const QString &key, const QString &data);
    void commit(const QString &commitMessage);
    void readKey(const QString &key, const QString &revision);
private slots:
    void handleFinishedAppendingJsonObjectToTimeline_aka_emitCommitFinished(bool success);
};

#endif // STUPIDKEYVALUECONTENTTRACKER_H
