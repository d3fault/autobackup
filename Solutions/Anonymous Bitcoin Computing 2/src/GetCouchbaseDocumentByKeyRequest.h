#ifndef GETCOUCHBASEDOCUMENTBYKEYREQUEST_H
#define GETCOUCHBASEDOCUMENTBYKEYREQUEST_H

#include "Wt/WServer"
using namespace Wt;

#include "frontend/anonymousbitcoincomputingwtgui.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

class GetCouchbaseDocumentByKeyRequest
{
public:
    //save constructor
    GetCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string couchbaseGetKeyInput)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseGetKeyInput(couchbaseGetKeyInput) //, CouchbaseGetValueOutput(couchbaseGetValueOutput)
    { }
    //load constructor
    GetCouchbaseDocumentByKeyRequest()
        : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL)
    { }

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseGetKeyInput;
    //std::string CouchbaseGetValueOutput;

    static inline void respond(GetCouchbaseDocumentByKeyRequest *originalRequest, const void *couchbaseDocument, size_t couchbaseDocumentSizeBytes)
    {
         Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::getCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2), originalRequest->CouchbaseGetKeyInput, std::string((const char*)couchbaseDocument, couchbaseDocumentSizeBytes)));
    }
private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive &ar, const unsigned int version) const
    {
        (void)version;
        ar & WtSessionId;

        char AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI];
        memcpy(&AddressToAnonymousBitcoinComputingWtGUIAsString, &AnonymousBitcoinComputingWtGUIPointerForCallback, LengthOfAddressToAnonymousBitcoinComputingWtGUI);
        AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI-1] = '\0';
        ar & boost::serialization::make_binary_object(&AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI);

        ar & CouchbaseGetKeyInput;
        //ar & CouchbaseSetValue;
    }
    template<class Archive>
    void load(Archive &ar, const unsigned int version)
    {
        (void)version;
        ar & WtSessionId;

        char AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI];
        ar & boost::serialization::make_binary_object(&AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI);
        memcpy(&AnonymousBitcoinComputingWtGUIPointerForCallback, &AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI-1);

        ar & CouchbaseGetKeyInput;
        //ar & CouchbaseSetValue;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // GETCOUCHBASEDOCUMENTBYKEYREQUEST_H

