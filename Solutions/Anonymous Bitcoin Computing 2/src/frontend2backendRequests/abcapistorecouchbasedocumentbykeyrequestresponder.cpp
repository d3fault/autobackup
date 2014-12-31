#include "abcapistorecouchbasedocumentbykeyrequestresponder.h"

#include <QObject>

void AbcApiStoreCouchbaseDocumentByKeyRequestResponder::respond(bool lcbStoreModeIsAdd_UNUSED, const std::string &wtSessionId_UNUSED, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseStoreKeyInput_UNUSED, bool lcbOpSuccess, bool dbError)
{
    (void)lcbStoreModeIsAdd_UNUSED;
    (void)wtSessionId_UNUSED;
    (void)couchbaseStoreKeyInput_UNUSED;
    QMetaObject::invokeMethod(static_cast<QObject*>(getTodaysAdSlotServerClientConnection), "backendDbStoreCallback", Qt::QueuedConnection, Q_ARG(bool, lcbOpSuccess), Q_ARG(bool, dbError));
}
void AbcApiStoreCouchbaseDocumentByKeyRequestResponder::respondWithCas(const std::string &wtSessionId, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseStoreKeyInput, u_int64_t casOutput, bool lcbOpSuccess, bool dbError)
{
    //TODOoptional: API doesn't use cas atm, so fuck it
    qFatal("AbcApiStoreCouchbaseDocumentByKeyRequestResponder::respondWithCas not implemented");
}
