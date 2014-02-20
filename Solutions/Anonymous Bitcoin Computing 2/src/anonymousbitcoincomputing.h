#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#include <Wt/WServer>
using namespace Wt;
using namespace std;

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread.hpp>

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#define ABC_TELL_WT_ABOUT_THE_LIBEVENTS_WE_SET_UP_FOR_IT_TO_SEND_MESSAGES_TO_COUCHBASE_MACRO(z, n, text) \
AnonymousBitcoinComputingWtGUI::m_##text##EventCallbacksForWt[n] = couchbaseDb.get##text##EventCallbackForWt##n();

/////////////////////////////////////////////////////END MACRO HELL/////////////////////////////////////////////////

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
