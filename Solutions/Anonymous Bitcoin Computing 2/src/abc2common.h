#ifndef ABC2COMMON_H
#define ABC2COMMON_H

#include <list>
#include <string>

#include <boost/version.hpp>
#include <boost/shared_ptr.hpp> //ViewQueryPageContentsType

#define ABC_500_INTERNAL_SERVER_ERROR_MESSAGE "500 Internal Server Error"
#define ABC_WT_COUCHBASE_MESSAGE_QUEUES_BASE_NAME "AbcWtToCouchbaseMessageQueuesBaseName"
#define ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS 576
#define ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS 96

#define ABC2_BITCOIN_AMOUNT_VISUAL_INPUT_FORM_WIDTH 17 /*16 digits +1 for decimal place*/

#if (BOOST_VERSION >= 105300)
#define ABC_USE_BOOST_LOCKFREE_QUEUE
#endif

#ifdef ABC_USE_BOOST_LOCKFREE_QUEUE
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS 3
//#define ABC_NAME_OF_STORE_LARGE_NOT_NEEDED_ANYMORE_SINCE_LOCKFREE_QUEUE_HACK Store //hack used to redirect all frontend->backend "StoreLarge" requests to use the regular "Store" queue, when in lockfree::queue mode. This hack doesn't need to be utilized on the backend, since the StoreLarge libevents are never raised. eh this hack ended up causing macro expansion problems, so the relevant portions just use ABC_USE_BOOST_LOCKFREE_QUEUE stuffs instead..
#else
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SETS 4 //if you add more message queue types, increment the 3 above and the 4 here, and make sure StoreLarge remains the last one
#define ABC_NAME_OF_STORE_LARGE_NOT_NEEDED_ANYMORE_SINCE_LOCKFREE_QUEUE_HACK StoreLarge
#endif

//#define ABC_TESTING_SIZE_VARIABLES_PLZ
#ifdef ABC_TESTING_SIZE_VARIABLES_PLZ
//TESTING VARIABLES (valgrind is slooooooow)

//set 0 -- Store
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET0 Store
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET0 5
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store 5
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Store 2
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Store 2048

//set 1 -- Get
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET1 Get
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET1 3
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Get 3
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Get 3
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Get 500

//set 2 -- View Query
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET2 ViewQuery
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET2 5
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_ViewQuery 5
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_ViewQuery 2
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_ViewQuery 500

#ifndef ABC_USE_BOOST_LOCKFREE_QUEUE
//set 3 -- Store Large
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET3 StoreLarge
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET3 2
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_StoreLarge 2
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_StoreLarge 1
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_StoreLarge 2157392
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#define ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT 0
#define ABC_COUCHBASE_DURABILITY_WAIT_FOR_PERSISTED_COUNT 1 //just to give durability_poll something to do on my 1-node cluster test environment
#define ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE 4
#define ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE 3
#define ABC_NUM_BITCOINKEYSETS 2

#else //DEPLOY VARIABLES
//CORE TUNABLES (message queue count, message queue max messages, message queue max message size)

//set 0 -- Store
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET0 Store
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET0 25
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Store 25
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Store 5
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Store 10240 //10kb per store is probably plenty...

//set 1 -- Get
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET1 Get
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET1 125
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_Get 125
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_Get 50
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_Get 500 //had '251' to reflect max key size, but that doesn't account for the rest of the message wt->couchbase message. When I'm done hacking shit in, I should do 251+overhead. problem is i dunno how much space boost serialization is using :-P

//set 2 -- View Query
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET2 ViewQuery
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET2 100
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_ViewQuery 100
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_ViewQuery 5
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_ViewQuery 500

#ifndef ABC_USE_BOOST_LOCKFREE_QUEUE
//set 3 -- Store Large
#define ABC_NAME_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_SET3 StoreLarge
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_SET3 6
#define ABC_NUMBER_OF_WT_TO_COUCHBASE_MESSAGE_QUEUES_IN_StoreLarge 6
#define ABC_MAX_NUMBER_OF_WT_TO_COUCHBASE_MESSAGES_IN_EACH_QUEUE_FOR_StoreLarge 2
#define ABC_SIZE_OF_WT_TO_COUCHBASE_MESSAGE_QUEUE_MESSAGES_FOR_StoreLarge 2157392 //~2.5mb, to account for overhead + b64encoding
#endif // ABC_USE_BOOST_LOCKFREE_QUEUE

#define ABC_COUCHBASE_DURABILITY_WAIT_FOR_REPLICACTION_COUNT 2
#define ABC_NUM_BITCOIN_KEY_RANGES_ON_EACH_HUGEBITCOINLIST_PAGE 100
#define ABC_NUM_BITCOIN_KEYS_ON_EACH_BITCOINKEYSET_PAGE 100 //also is number of bitcoin keys in each range on a hugeBitcoinList page
#define ABC_NUM_BITCOINKEYSETS 1000
#endif // ABC_TESTING_SIZE_VARIABLES_PLZ

//MIN MEMORY REQUIRED [for message queues only] calculated via above tunables: QUEUE_MAX_MESSAGE_SIZE * MAX_MESSAGES_IN_QUEUE * NUMBER_OF_QUEUES

typedef std::list<std::string> ViewQueryPageContentsPointedType;
typedef boost::shared_ptr<ViewQueryPageContentsPointedType> ViewQueryPageContentsType; //TODOoptional: search for any/all uses of "delete" application-wide and use a shared pointer instead

#endif // ABC2COMMON_H
