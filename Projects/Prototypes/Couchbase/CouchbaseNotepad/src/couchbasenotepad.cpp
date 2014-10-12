#include "couchbasenotepad.h"

#include <QCoreApplication>

const struct timeval CouchbaseNotepad::m_ThirtyMilliseconds = {0,30000};

//copy/pasted liberally from anonymousbitcoincomputingcouchbasedb.cpp
CouchbaseNotepad::CouchbaseNotepad(QObject *parent)
    : QObject(parent)
    , m_PendingGetCount(0)
    , m_PendingStoreCount(0)
{ }
void CouchbaseNotepad::errorCallbackStatic(lcb_t instance, lcb_error_t error, const char *errinfo)
{
    CouchbaseNotepad *couchbaseNotepad = const_cast<CouchbaseNotepad*>(static_cast<const CouchbaseNotepad*>(lcb_get_cookie(instance)));
    couchbaseNotepad->errorCallback(error, errinfo);
}
void CouchbaseNotepad::errorCallback(lcb_error_t error, const char *errinfo)
{
    std::string errInfoStdString(errinfo, strlen(errinfo));
    emit e("COUCHBASE ERROR CALLBACK: " + QString::fromStdString(lcb_strerror(m_Couchbase, error)) + " - " + QString::fromStdString(errinfo));
}
void CouchbaseNotepad::configurationCallbackStatic(lcb_t instance, lcb_configuration_t config)
{
    CouchbaseNotepad *couchbaseNotepad = const_cast<CouchbaseNotepad*>(static_cast<const CouchbaseNotepad*>(lcb_get_cookie(instance)));
    couchbaseNotepad->configurationCallback(config);
}
void CouchbaseNotepad::configurationCallback(lcb_configuration_t config)
{
    if(config == LCB_CONFIGURATION_NEW)
    {
        emit initializedAndStartedSuccessfully();
    }
}
void CouchbaseNotepad::storeCallbackStatic(lcb_t instance, const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    CouchbaseNotepad *couchbaseNotepad = const_cast<CouchbaseNotepad*>(static_cast<const CouchbaseNotepad*>(lcb_get_cookie(instance)));
    couchbaseNotepad->storeCallback(cookie, operation, error, resp);
}
void CouchbaseNotepad::storeCallback(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    storeCallbackActual(cookie, operation, error, resp);
    --m_PendingStoreCount;
}
void CouchbaseNotepad::storeCallbackActual(const void *cookie, lcb_storage_t operation, lcb_error_t error, const lcb_store_resp_t *resp)
{
    Q_UNUSED(operation)
    Q_UNUSED(cookie)

    std::string keyStdString(static_cast<const char*>(resp->v.v0.key), resp->v.v0.nkey);
    QString keyQString = QString::fromStdString(keyStdString);

    if(error != LCB_SUCCESS)
    {
        QString value;

        if(error == LCB_KEY_EEXISTS)
        {
            //if LCB_ADD'ing, the key already exists. If LCB_SET, then your cas (which is implied otherwise we can't get this error) didn't match when setting (someone modified since you did the get)

#if 0 //later
            if(originalRequest->SaveCasOutput)
            {
                StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, resp->v.v0.cas, false, false);
            }
            else
            {
                StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, false);
            }
#endif
            emit addCouchbaseNotepadDocFinished(false, false, keyQString, value);
            return;
        }

        //db error
        emit addCouchbaseNotepadDocFinished(true, false, keyQString, value);
    }

#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
    lcb_durability_opts_t lcbDurabilityOptions;
    memset(&lcbDurabilityOptions, 0, sizeof(lcb_durability_opts_t));
    //lcbDurabilityOptions.v.v0.persist_to = 1;
    lcbDurabilityOptions.v.v0.replicate_to = 2;
    lcbDurabilityOptions.v.v0.timeout = 5000000; //5 second timeout
    lcb_durability_cmd_t lcbDurabilityCommand;
    memset(&lcbDurabilityCommand, 0, sizeof(lcb_durability_cmd_t));
    lcbDurabilityCommand.v.v0.key = resp->v.v0.key;
    lcbDurabilityCommand.v.v0.nkey = resp->v.v0.nkey;

#if 0 //later
    if(originalRequest->HasCasInput)
    {
        //LCB_SET implied
        lcbDurabilityCommand.v.v0.cas = resp->v.v0.cas;
    }
    if(originalRequest->SaveCasOutput)
    {
        originalRequest->CasInput = resp->v.v0.cas; //HACK (1st): storing this here might give me pain later, but eh basically i need to store it somewhere because i don't have access to it in the durability callback.... and eh i'm PRETTY sure i'm done with the casInput at this point... so i think it's safe to use this field for just a little
    }
#endif

    lcb_durability_cmd_t *lcbDurabilityCommandList[1];
    lcbDurabilityCommandList[0] = &lcbDurabilityCommand;
    error = lcb_durability_poll(m_Couchbase, cookie, &lcbDurabilityOptions, 1, lcbDurabilityCommandList);
    if(error != LCB_SUCCESS)
    {
        emit e("Failed to schedule lcb_durability_poll: " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
#if 0 //later
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, 0, false, true);
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, false, true);
        }
#endif
        QString value;
        emit addCouchbaseNotepadDocFinished(true, false, keyQString, value);
        return;
    }
#else //no durability polling, just respond
    if(originalRequest->SaveCasOutput)
    {
        StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, resp->v.v0.cas, true, false);
    }
    else
    {
        StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, false);
    }
    delete originalRequest;
#endif // ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
}
void CouchbaseNotepad::getCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    CouchbaseNotepad *couchbaseNotepad = const_cast<CouchbaseNotepad*>(static_cast<const CouchbaseNotepad*>(lcb_get_cookie(instance)));
    couchbaseNotepad->getCallback(cookie, error, resp);
}
void CouchbaseNotepad::getCallback(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    getCallbackActual(cookie, error, resp);
    --m_PendingGetCount;
}
void CouchbaseNotepad::getCallbackActual(const void *cookie, lcb_error_t error, const lcb_get_resp_t *resp)
{
    Q_UNUSED(cookie)
    std::string keyStdString(static_cast<const char*>(resp->v.v0.key), resp->v.v0.nkey);
    QString keyQString = QString::fromStdString(keyStdString);
    if(error != LCB_SUCCESS)
    {
        QString value;

        if(error == LCB_KEY_ENOENT)
        {
            //key not found
            emit getCouchbaseNotepadDocFinished(false, false, keyQString, value);
            return;
        }

        //db error
        emit getCouchbaseNotepadDocFinished(true, false, keyQString, value);
        return;
    }

#if 0 //later
    if(originalRequest->SaveCAS) //TODOoptimization: __unlikely or whatever i can find in boost
    {
        GetCouchbaseDocumentByKeyRequest::respondWithCAS(originalRequest, std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes), resp->v.v0.cas, true, false);
    }
    else
    {
        GetCouchbaseDocumentByKeyRequest::respond(originalRequest, std::string(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes), true, false);
    }
#endif

    std::string valueStdString(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString valueQString = QString::fromStdString(valueStdString);
    emit getCouchbaseNotepadDocFinished(false, true, keyQString, valueQString);
}
void CouchbaseNotepad::durabilityCallbackStatic(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    CouchbaseNotepad *couchbaseNotepad = const_cast<CouchbaseNotepad*>(static_cast<const CouchbaseNotepad*>(lcb_get_cookie(instance)));
    couchbaseNotepad->durabilityCallback(cookie, error, resp);
}
void CouchbaseNotepad::durabilityCallback(const void *cookie, lcb_error_t error, const lcb_durability_resp_t *resp)
{
    Q_UNUSED(cookie)
    std::string keyStdString(static_cast<const char*>(resp->v.v0.key), resp->v.v0.nkey);
    QString keyQString = QString::fromStdString(keyStdString);

    if(error != LCB_SUCCESS)
    {
        emit e("Error: Generic lcb_durability_poll callback failure: " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
#if 0 //later
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, true); //another use of that cas hack (3rd)
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, true);
        }
#endif
        //TODOreq: if we make it to durability, the lcb op has succeeded?
        //^Yes, but what the fuck are the implications of that??? My business logic for now assumes that db error lcb op failed (it doesn't even check it). It's especially worth noting that a 'recovery possy' member might see an LCB_ADD whose replication failed (here) and then do a bunch of recovery steps.... and/but our business will be out of sync in some obscure way I have never thought about (maybe it doesn't mess anything up, maybe the world ends)
        QString value;
        emit addCouchbaseNotepadDocFinished(true, false /*or true? idfk*/, keyQString, value);
        return;
    }
    if(resp->v.v0.err != LCB_SUCCESS)
    {
        emit e("Error: Specific lcb_durability_poll callback failure: " + QString::fromStdString(lcb_strerror(m_Couchbase, resp->v.v0.err)));
#if 0 //later
        if(originalRequest->SaveCasOutput)
        {
            StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, true); //another use of that cas hack (4th)
        }
        else
        {
            StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, true);
        }
#endif
        //TODOoptional: if we make it to durability, the lcb op has succeeded?
        //^Yes, see same req above for concerns
        QString value;
        emit addCouchbaseNotepadDocFinished(true, false /*or true? idfk*/, keyQString, value);
        return;
    }

#if 0 //later
    if(originalRequest->SaveCasOutput)
    {
        StoreCouchbaseDocumentByKeyRequest::respondWithCas(originalRequest, originalRequest->CasInput, true, false); //it's a HACK (2nd) that you can see in storeCallback, my using of the casInput field to hold onto (until we get to here, the durability callback) what is actually the cas OUTPUT. since you see this request is about to be deleted in a few lines (and we don't use the serializer to get back to Wt, we use a .Post, this appears safe
    }
    else
    {
        StoreCouchbaseDocumentByKeyRequest::respond(originalRequest, true, false);
    }
#endif
    QString durabilitySuccessLoLValue = "bleh if you see this code harder"; //we probably won't rely on the returned value for a store anyways, and i'm too lazy to pass it along through the cookie
    emit addCouchbaseNotepadDocFinished(false, true, keyQString, durabilitySuccessLoLValue);
}
void CouchbaseNotepad::callQAppProcessEventsStatic(int unusedSocket, short events, void *userData)
{
    Q_UNUSED(unusedSocket)
    Q_UNUSED(events)
    const_cast<CouchbaseNotepad*>(static_cast<const CouchbaseNotepad*>(userData))->callQAppProcessEvents();
}
void CouchbaseNotepad::callQAppProcessEvents()
{
    qApp->processEvents();
    event_add(m_ProcessQtEventsTimeout, &m_ThirtyMilliseconds);
}
void CouchbaseNotepad::initializeAndStart()
{
    //libevent
    struct LibEventBaseScopedDeleter
    {
        bool LibEventBaseInitializeSuccess;
        struct event_base *LibEventBase;

        LibEventBaseScopedDeleter()
            : LibEventBaseInitializeSuccess(true), LibEventBase(event_base_new())
        {
            if(!LibEventBase)
            {
                LibEventBaseInitializeSuccess = false;
            }
        }
        ~LibEventBaseScopedDeleter()
        {
            if(LibEventBaseInitializeSuccess)
            {
                event_base_free(LibEventBase);
            }
        }
    } LibEventBaseScopedDeleterInstance;

    if(!LibEventBaseScopedDeleterInstance.LibEventBaseInitializeSuccess)
    {
        emit e("Error: Libevent failed to initialize");
        emit couchbaseNotepadExitted();
        return;
    }

#if 0 //qt will be handling the cross thread stuff, thanks
    if(evthread_use_pthreads() < 0) //or evthread_use_windows_threads
    {
        emit e("Error: Failed to evthread_use_pthreads");
        emit couchbaseNotepadExitted(false);
        return;
    }
    if(evthread_make_base_notifiable(LibEventBaseScopedDeleterInstance.LibEventBase) < 0)
    {
        emit e("Error: Failed to make event base notifiable");
        emit couchbaseNotepadExitted(false);
        return;
    }
#endif
    struct lcb_create_io_ops_st couchbaseCreateIoOps;
    struct CouchbaseIoOpsScopedDeleter
    {
        bool DontDestroyBecauseCreateFailed;
        lcb_io_opt_t CouchbaseIoOps;
        CouchbaseIoOpsScopedDeleter()
            : DontDestroyBecauseCreateFailed(false)
        { }
        ~CouchbaseIoOpsScopedDeleter()
        {
            if(!DontDestroyBecauseCreateFailed)
            {
                lcb_destroy_io_ops(CouchbaseIoOps);
            }
        }
    } CouchbaseIoOpsScopedDeleterInstance;
    lcb_error_t error;
    memset(&couchbaseCreateIoOps, 0, sizeof(couchbaseCreateIoOps));
    couchbaseCreateIoOps.v.v0.type = LCB_IO_OPS_LIBEVENT;
    couchbaseCreateIoOps.v.v0.cookie = LibEventBaseScopedDeleterInstance.LibEventBase;
    error = lcb_create_io_ops(&CouchbaseIoOpsScopedDeleterInstance.CouchbaseIoOps, &couchbaseCreateIoOps);
    if(error != LCB_SUCCESS)
    {
        CouchbaseIoOpsScopedDeleterInstance.DontDestroyBecauseCreateFailed = true;
        emit e("Error: Failed to create an IOOPS structure for libevent: " + QString::fromStdString(lcb_strerror(NULL, error)));
        emit couchbaseNotepadExitted();
        return;
    }

    //couchbase
    struct lcb_create_st copts;
    memset(&copts, 0, sizeof(copts));
    copts.v.v0.host = "192.168.56.10:8091";
    copts.v.v0.io = CouchbaseIoOpsScopedDeleterInstance.CouchbaseIoOps;
    if((error = lcb_create(&m_Couchbase, &copts)) != LCB_SUCCESS)
    {
        emit e("Failed to create a libcouchbase instance: " + QString::fromStdString(lcb_strerror(NULL, error)));
        emit couchbaseNotepadExitted();
        return;
    }

    lcb_set_cookie(m_Couchbase, this);

    //couchbase callbacks
    lcb_set_error_callback(m_Couchbase, CouchbaseNotepad::errorCallbackStatic);
    lcb_set_configuration_callback(m_Couchbase, CouchbaseNotepad::configurationCallbackStatic);
    lcb_set_get_callback(m_Couchbase, CouchbaseNotepad::getCallbackStatic);
    lcb_set_store_callback(m_Couchbase, CouchbaseNotepad::storeCallbackStatic);
#ifdef ABC_DO_COUCHBASE_DURABILITY_POLL_BEFORE_CONSIDERING_STORE_COMPLETE
    lcb_set_durability_callback(m_Couchbase, CouchbaseNotepad::durabilityCallbackStatic);
#endif
    if((error = lcb_connect(m_Couchbase)) != LCB_SUCCESS)
    {
        emit e("Failed to start connecting libcouchbase instance: " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
        lcb_destroy(m_Couchbase);
        emit couchbaseNotepadExitted();
        return;
    }

    //libevent events

    m_ProcessQtEventsTimeout = evtimer_new(LibEventBaseScopedDeleterInstance.LibEventBase, &CouchbaseNotepad::callQAppProcessEventsStatic, this);

#if 0 //eh, unneeded?
    m_GetEventCallback = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, libEventSlotForGetStatic, this);
    m_StoreEventCallback = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, libEventSlotForStoreStatic, this);
#endif

#if 0 //yes i want this in this prototype, but not yet...
    AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setAnonymousBitcoinComputingCouchbaseDB(this);
    AutoRetryingWithExponentialBackoffCouchbaseGetRequest::setEventBase(LibEventBaseScopedDeleterInstance.LibEventBase); //need this as a member to add exponential backoff timers at runtime
#endif

#if 0 // i think this is boost specific thread coordination stuff, leaving until i'm sure
    m_BeginStoppingCouchbaseCleanlyEvent = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, &AnonymousBitcoinComputingCouchbaseDB::beginStoppingCouchbaseCleanlyEventSlotStatic, this);
    m_FinalCleanUpAndJoinEvent = event_new(LibEventBaseScopedDeleterInstance.LibEventBase, -1, EV_READ, &AnonymousBitcoinComputingCouchbaseDB::finalCleanupAndJoin, LibEventBaseScopedDeleterInstance.LibEventBase);
    m_IsInEventLoop = true; //race conditions? cached results? fuck the police
#endif

    event_add(m_ProcessQtEventsTimeout, &m_ThirtyMilliseconds);

    event_base_loop(LibEventBaseScopedDeleterInstance.LibEventBase, 0); //wait until event_base_loopbreak is called, processing all events of course. TODOreq: qApp->processEvents() periodically, i guess a 30ms libevent timer?
    event_free(m_ProcessQtEventsTimeout);
    lcb_destroy(m_Couchbase);
    emit couchbaseNotepadExitted(true);
}
void CouchbaseNotepad::getCouchbaseNotepadDocByKey(const QString &key)
{
    lcb_get_cmd_t cmd;
    const lcb_get_cmd_t *cmds[1];
    cmds[0] = &cmd;
    std::string keyStdString = key.toStdString();
    memset(&cmd, 0, sizeof(cmd));
    cmd.v.v0.key = keyStdString.c_str();
    cmd.v.v0.nkey = keyStdString.length();
    lcb_error_t error = lcb_get(m_Couchbase, 0, 1, cmds);
    if(error  != LCB_SUCCESS)
    {
        emit e("Failed to setup get request: " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
        QString value;
        emit getCouchbaseNotepadDocFinished(true, false, key, value);
        return;
    }
    ++m_PendingGetCount;
}
void CouchbaseNotepad::storeCouchbaseNotepadDocByKey(const QString &key, const QString &value, bool overwriteExisting)
{
    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t *cmds[1];
    cmds[0] = &cmd;
    memset(&cmd, 0, sizeof(cmd));
    std::string keyStdString = key.toStdString();
    std::string valueStdString = value.toStdString();
    cmd.v.v0.key = keyStdString.c_str();
    cmd.v.v0.nkey = keyStdString.length();
    cmd.v.v0.bytes = valueStdString.c_str();
    cmd.v.v0.nbytes = valueStdString.length();
    if(!overwriteExisting)
    {
        cmd.v.v0.operation = LCB_ADD;
    }
    else
    {
        cmd.v.v0.operation = LCB_SET;
#if 0 //later
        if(originalRequest->HasCasInput)
        {
            cmd.v.v0.cas = originalRequest->CasInput;
        }
#endif
    }
    lcb_error_t err = lcb_store(m_Couchbase, 0, 1, cmds);
    if(err != LCB_SUCCESS)
    {
        emit e("Failed to set up store request: " + QString::fromStdString(lcb_strerror(m_Couchbase, err)));
        emit addCouchbaseNotepadDocFinished(true, false, key, value);
        return;
    }
    ++m_PendingStoreCount;
}
