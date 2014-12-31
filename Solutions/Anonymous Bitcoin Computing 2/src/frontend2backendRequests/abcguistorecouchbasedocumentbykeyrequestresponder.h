#ifndef ABCGUISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
#define ABCGUISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H

#include "istorecouchbasedocumentbykeyrequestresponder.h"

class AbcGuiStoreCouchbaseDocumentByKeyRequestResponder : public IStoreCouchbaseDocumentByKeyRequestResponder
{
public:
    void respond(bool lcbStoreModeIsAdd, const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseStoreKeyInput, bool lcbOpSuccess, bool dbError);
    void respondWithCas(const std::string &wtSessionId, void *anonymousBitcoinComputingWtGUIPointerForCallback, const std::string &couchbaseStoreKeyInput, u_int64_t casOutput, bool lcbOpSuccess, bool dbError);
};

#endif // ABCGUISTORECOUCHBASEDOCUMENTBYKEYREQUESTRESPONDER_H
