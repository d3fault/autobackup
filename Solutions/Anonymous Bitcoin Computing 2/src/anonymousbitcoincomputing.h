#ifndef ANONYMOUSBITCOINCOMPUTING_H
#define ANONYMOUSBITCOINCOMPUTING_H

#include <Wt/WServer>
using namespace Wt;
using namespace std;

#include <boost/preprocessor/repeat.hpp>
#include <boost/thread.hpp>

/////////////////////////////////////////////////////BEGIN MACRO HELL///////////////////////////////////////////////

#define TELL_WT_ABOUT_THE_LIBEVENTS_WE_SET_UP_FOR_IT_TO_SEND_MESSAGES_TO_COUCHBASE_MACRO(z, n, text) \
AnonymousBitcoinComputingWtGUI::m_##text##EventCallbacksForWt[n] = AnonymousBitcoinComputingCouchbaseDBScopedDeleterInstance.CouchbaseDb->get##text##EventCallbackForWt##n();

/////////////////////////////////////////////////////END MACRO HELL/////////////////////////////////////////////////

//#define WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME "AbcWtToCouchbaseMessageQueuesBaseName"

//#define NUMBER_OF_WT_TO_COUCHBASE_ADD_MESSAGE_QUEUES 25
//#define NUMBER_OF_WT_TO_COUCHBASE_GET_MESSAGE_QUEUES 125

//ADD
//#define WT_TO_COUCHBASE_ADD_MAX_MESSAGES_IN_QUEUE 200 //this is multiplied by the number of message queues, so 200 is PLENTY
//#define WT_TO_COUCHBASE_ADD_MAX_MESSAGE_SIZE 102400 //100kb seems reasonable for most all "ADD" requests, and perhaps we can use a much lower amount of "2-20mb" max sized queues for when they are [rarely] needed. Like 1 or 2 of them only...

//GET
//#define WT_TO_COUCHBASE_GET_MAX_MESSAGES_IN_QUEUE 1000 //(5x size of add, just like queue_count)
//#define WT_TO_COUCHBASE_GET_MAX_MESSAGE_SIZE 102400 //100kb just like Add, see Add's notes about larger messages

//MIN MEMORY REQUIRED [for message queues only] calculated via above tunables: QUEUE_MAX_MESSAGE_SIZE * MAX_MESSAGES_IN_QUEUE * NUMBER_OF_QUEUES

class AnonymousBitcoinComputing
{
public:
    int startAbcAndWaitForFinished(int argc, char **argv);
private:
    static WApplication *createAnonymousBitcoinComputingWtGUI(const WEnvironment &env);
};

#endif // ANONYMOUSBITCOINCOMPUTING_H
