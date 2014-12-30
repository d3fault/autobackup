#ifndef ABCAPIGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
#define ABCAPIGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H

#include "igetcouchbasedocumentbykeyrequestresponder.h"

class AbcApiGetCouchbaseDocumentByKeyRequestResponder : public IGetCouchbaseDocumentByKeyRequestResponder
{
public:
    void respond(const std::string &wtSessionId_UNUSED, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void respondWithCAS(unsigned char getAndSubscribe_UNUSED, const std::string &wtSessionId_UNUSED, void *GetTodaysAdSlotServerClientConnection, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);
};

#endif // ABCAPIGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
