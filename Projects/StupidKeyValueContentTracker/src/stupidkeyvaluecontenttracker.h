#ifndef STUPIDKEYVALUECONTENTTRACKER_H
#define STUPIDKEYVALUECONTENTTRACKER_H

#include <QObject>

#include <QHash>
#include <QSharedPointer>
#include <QJsonObject>

class IKeyValueStoreMutation;

typedef QSharedPointer<IKeyValueStoreMutation> StupidKeyValueContentTracker_StagedMutationsValueType;

class StupidKeyValueContentTracker : public QObject
{
    Q_OBJECT
public:
    explicit StupidKeyValueContentTracker(QObject *parent = 0);
private:
    bool commitStagedKeyValueStoreMutations(const QString &commitMessage);
    void populateCommitDataUsingStagedKeyValueStoreMutations(QJsonObject &bulkMutations);
    bool commitActual(const QJsonObject &commitData, const QString &commitMessage);

    QHash<QString /*key*/, StupidKeyValueContentTracker_StagedMutationsValueType /*value*/> m_StagedKeyValueStoreMutation; //TODOreq: Implicit Sharing instead of QSharedPointer? implicit sharing is similar but safer/better (takes longer to set up tho)!!
signals:
    void e(const QString &msg);
    void o(const QString &msg);

    void addFinished(bool success);
    void commitFinished(bool success);
    void readKeyFinished(bool success, const QString &QString, const QString &revision, const QString &data);
public slots:
    void add(const QString &key, const QString &data);
    void commit(const QString &commitMessage);
    void readKey(const QString &key, const QString &revision);
};

#endif // STUPIDKEYVALUECONTENTTRACKER_H
