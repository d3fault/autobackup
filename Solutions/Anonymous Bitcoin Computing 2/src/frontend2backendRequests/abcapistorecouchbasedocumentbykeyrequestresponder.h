#ifndef ABCAPISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
#define ABCAPISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H

#include "istorecouchbasedocumentbykeyrequestresponder.h"

class AbcApiStoreCouchbaseDocumentByKeyRequestResponder : public IStoreCouchbaseDocumentByKeyRequestResponder
{
public:
    void respond(bool lcbStoreModeIsAdd_UNUSED, const std::string &wtSessionId_UNUSED, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseStoreKeyInput, bool lcbOpSuccess, bool dbError);
    void respondWithCas(const std::string &wtSessionId, void *getTodaysAdSlotServerClientConnection, const std::string &couchbaseStoreKeyInput, u_int64_t casOutput, bool lcbOpSuccess, bool dbError);
};

#endif // ABCAPISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
