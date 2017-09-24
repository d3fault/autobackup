#include "stupidkeyvaluecontenttracker.h"

#include <QDateTime>
#include <QHashIterator>

#include "keyvaluestoremutation_add.h"

//TODOreq: this class uses (wraps/abstracts/hides) "mutations" of a KeyValue store. all underlying data must fit into a TimeAndData_Timeline json doc. Data is b64 encoded in that title, we do NOT add keys "next to" 'time' and 'data'. data will be, in MOST OF MY USES, a json sub-obj! I had written "a b64 json obj", but actually that's not necessary since json has the same "escape keys" as json (duh). fuck yea +1 json. but still worth noting that time and data are the ONLY 2 top level keys
//TODOoptional: libfuse could present it as a fs... key = filepath, value = file contents. hahaha and these fs dev noobs are STILL making "fs attribute extension" shit left and right hahahaha (doing it wrong)
StupidKeyValueContentTracker::StupidKeyValueContentTracker(QObject *parent)
    : QObject(parent)
{ }
bool StupidKeyValueContentTracker::commitStagedKeyValueStoreMutations(const QString &commitMessage)
{
    //TODOreq: all or nothing! I mean that IS the point of a commit... but still I doubt I'll see failures at first, those are corner cases
    //nvm: qint64 currentMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); //time in the TimeAndData_Timeline context (nvm)!!!
    QJsonObject commitData; //nvm: data in the TimeAndData_Timeline context (nvm)!!! <-- we still need to squeeze the "commit message" into the "data" in the TimeAndData_Timeline, and we're not there quite yet
    populateCommitDataUsingStagedKeyValueStoreMutations(commitData);
    return commitActual(commitData, commitMessage);
}
void StupidKeyValueContentTracker::populateCommitDataUsingStagedKeyValueStoreMutations(QJsonObject &bulkMutations)
{
    QHashIterator<QString, StupidKeyValueContentTracker_StagedMutationsValueType> it(m_StagedKeyValueStoreMutation);
    while(it.hasNext())
    {
        it.next();
        IKeyValueStoreMutation *mutation = it.value().data();
        mutation->appendYourselfToBulkMutationsJsonObject(it.key(), bulkMutations);
    }
}
bool StupidKeyValueContentTracker::commitActual(const QJsonObject &commitData, const QString &commitMessage)
{
    QJsonObject commitDataActual;
    commitDataActual.insert(StupidKeyValueContentTracker_JSONKEY_COMMITMESSAGE, commitMessage);
    commitDataActual.insert(StupidKeyValueContentTracker_JSONKEY_BULKMUTATIONS, QJsonValue(commitData));
    qint64 currentMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch(); //time in the TimeAndData_Timeline context!!!

    //TODOreq: write that BulkMutations to TimeAndData_Timeline.... somehow...
    //something like:
    return m_Timeline.tryAppend(currentMSecsSinceEpoch, commitDataActual);
    //^maybe a signal appendToTimelineRequested? sameshit but yea. but the way we got here though was a synchronous/error-checking private call, so maybe NOT using a signal is best? BUT tbh async is almost always better, so even though it'd be a small refactor async is def doable. ima actually CODE m_Timeline before I decide...
}
void StupidKeyValueContentTracker::add(const QString &key, const QString &data)
{
    if(m_StagedKeyValueStoreMutation.contains(key))
    {
        emit e("you are already mutating key '" + QString(key) + "', so commit first or unstage it");
        emit addFinished(false);
        return;
    }
    QSharedPointer<KeyValueStoreMutation_Add> addMutation(new KeyValueStoreMutation_Add(data)); //TODOreq: KeyValueStoreMutation_Delete, KeyValueStoreMutation_Modify
    m_StagedKeyValueStoreMutation.insert(key, addMutation);
    emit addFinished(true);
}
void StupidKeyValueContentTracker::commit(const QString &commitMessage)
{
    if(!commitStagedKeyValueStoreMutations(commitMessage))
    {
        emit e("error: failed to commitStagedKeyValueStoreMutation()! your key/values are still 'staged'");
        emit commitFinished(false);
        return;
    }
    emit o("successfully committed 'staged' key value store mutations");
    m_StagedKeyValueStoreMutation.clear();
    emit commitFinished(true);
}
void StupidKeyValueContentTracker::readKey(const QString &key, const QString &revision)
{
    if(revision.isEmpty())
    {
        //TODOreq: "HEAD" in git lingo
    }
    QString dataMaybe;
    bool found = myReadKey(key, revision, &dataMaybe);
    if(!found)
        emit e("key not found: '" + key(key) + "'");
    emit readKeyFinished(found, key, revision, dataMaybe);
}
