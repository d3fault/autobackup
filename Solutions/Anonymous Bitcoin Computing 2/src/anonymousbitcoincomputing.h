#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#include <Wt/WServer>
using namespace Wt;
using namespace std;

#include <boost/thread.hpp>

#include <QStringList>

class AnonymousBitcoinComputingCouchbaseDB;

class AnonymousBitcoinComputing
{
public:
    int startAbcAndWaitForFinished(int argc, char **argv);
private:
    static WApplication *createAnonymousBitcoinComputingWtGUI(const WEnvironment &env);
    void beginStoppingCouchbase(AnonymousBitcoinComputingCouchbaseDB *couchbaseDb);
    void finalStopCouchbaseAndWaitForItsThreadToJoin(AnonymousBitcoinComputingCouchbaseDB *couchbaseDb);

    inline int indexOfListItemThatStartsWith(const QStringList &listToFindFirstIndexOfStringThatStartsWith, const QString &startsWithString)
    {
        int i = 0;
        Q_FOREACH(const QString &currentListEntry, listToFindFirstIndexOfStringThatStartsWith)
        {
            if(currentListEntry.startsWith(startsWithString))
            {
                return i;
            }
            ++i;
        }
        return -1;
    }
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
