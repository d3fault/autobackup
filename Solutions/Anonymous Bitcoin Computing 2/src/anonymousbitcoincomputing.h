#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#include <Wt/WServer>
using namespace Wt;
using namespace std;

#include <boost/thread.hpp>

class AnonymousBitcoinComputingCouchbaseDB;

class AnonymousBitcoinComputing
{
public:
    int startAbcAndWaitForFinished(int argc, char **argv);
private:
    static WApplication *createAnonymousBitcoinComputingWtGUI(const WEnvironment &env);
    void beginStoppingCouchbase(AnonymousBitcoinComputingCouchbaseDB *couchbaseDb);
    void finalStopCouchbaseAndWaitForItsThreadToJoin(AnonymousBitcoinComputingCouchbaseDB *couchbaseDb);
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
