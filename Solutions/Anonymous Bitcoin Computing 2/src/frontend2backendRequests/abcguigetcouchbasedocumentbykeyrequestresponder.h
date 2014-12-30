#ifndef ABCGUIGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
#define ABCGUIGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H

#include "igetcouchbasedocumentbykeyrequestresponder.h"

class AbcGuiGetCouchbaseDocumentByKeyRequestResponder : public IGetCouchbaseDocumentByKeyRequestResponder
{
public:
    void respond(const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, bool lcbOpSuccess, bool dbError);
    void respondWithCAS(unsigned char getAndSubscribe, const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseGetKeyInput, const std::string &couchbaseDocument, u_int64_t cas, bool lcbOpSuccess, bool dbError);
};

#endif // ABCGUIGETCOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
