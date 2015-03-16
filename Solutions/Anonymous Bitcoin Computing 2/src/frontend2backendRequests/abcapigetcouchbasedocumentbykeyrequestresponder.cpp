#include "abcapigetcouchbasedocumentbykeyrequestresponder.h"

#include <QObject>

void AbcApiGetCouchbaseDocumentByKeyRequestResponder::respond(const std::string &wtSessionId_UNUSED, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    (void)wtSessionId_UNUSED;
    QMetaObject::invokeMethod(static_cast<QObject*>(getTodaysAdSlotServerClientConnection), "backendDbGetCallback", Qt::QueuedConnection, Q_ARG(std::string, couchbaseDocument), Q_ARG(bool, lcbOpSuccess), Q_ARG(bool, dbError));
}
void AbcApiGetCouchbaseDocumentByKeyRequestResponder::respondWithCAS(unsigned char getAndSubscribe_UNUSED, const std::string &wtSessionId_UNUSED, void *GetTodaysAdSlotServerClientConnection, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError)
{
    //TODOoptional: API doesn't use cas atm, so fuck it
    qFatal("AbcApiGetCouchbaseDocumentByKeyRequestResponder::respondWithCAS not implemented");
}
