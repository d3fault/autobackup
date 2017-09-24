#include "stupidkeyvaluecontenttracker.h"

//TODOreq: this class uses (wraps/abstracts/hides) "mutations" of a KeyValue store. all underlying data must fit into a TimeAndData_Timeline json doc. Data is b64 encoded in that title, we do NOT add keys "next to" 'time' and 'data'. data will be, in MOST OF MY USES, a json sub-obj! I had written "a b64 json obj", but actually that's not necessary since json has the same "escape keys" as json (duh). fuck yea +1 json. but still worth noting that time and data are the ONLY 2 top level keys
StupidKeyValueContentTracker::StupidKeyValueContentTracker(QObject *parent)
    : QObject(parent)
{ }
void StupidKeyValueContentTracker::add(const QByteArray &key, const QByteArray &data)
{
    m_StagedKeyValueStoreMutation.append(KeyValueStoreMutation_Add(key, data)); //TODOreq: KeyValueStoreMutation_Delete, KeyValueStoreMutation_Modify
    emit finishedAdd();
}
void StupidKeyValueContentTracker::commit(const QString &commitMessage)
{
    if(!commitStagedKeyValueStoreMutation(commitMessage))
    {
        emit e("error: failed to commitStagedKeyValueStoreMutation()! your key/values are still 'staged'");
        emit finishedCommit(false);
        return;
    }
    emit o("successfully committed 'staged' key value store mutations");
    m_StagedKeyValueStoreMutation.clear();
    emit finishedCommit(true);
}
