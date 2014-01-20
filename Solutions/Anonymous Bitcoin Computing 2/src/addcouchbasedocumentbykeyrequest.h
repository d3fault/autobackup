#ifndef ADDCOUCHBASEDOCUMENTBYKEYREQUEST_H
#define ADDCOUCHBASEDOCUMENTBYKEYREQUEST_H

#include "Wt/WServer"
using namespace Wt;

#include "frontend/anonymousbitcoincomputingwtgui.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

class AddCouchbaseDocumentByKeyRequest
{
public:
    //save constructor
    AddCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput)
    { }
    //load constructor
    AddCouchbaseDocumentByKeyRequest()
        : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL)
    { }

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseAddKeyInput;
    std::string CouchbaseAddDocumentInput;

    static inline void respond(AddCouchbaseDocumentByKeyRequest *originalRequest)
    {
         Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::addCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1), originalRequest->CouchbaseAddKeyInput));
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

        ar & CouchbaseAddKeyInput;
        ar & CouchbaseAddDocumentInput;
    }
    template<class Archive>
    void load(Archive &ar, const unsigned int version)
    {
        (void)version;
        ar & WtSessionId;

        char AddressToAnonymousBitcoinComputingWtGUIAsString[LengthOfAddressToAnonymousBitcoinComputingWtGUI];
        ar & boost::serialization::make_binary_object(&AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI);
        memcpy(&AnonymousBitcoinComputingWtGUIPointerForCallback, &AddressToAnonymousBitcoinComputingWtGUIAsString, LengthOfAddressToAnonymousBitcoinComputingWtGUI-1);

        ar & CouchbaseAddKeyInput;
        ar & CouchbaseAddDocumentInput;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // ADDCOUCHBASEDOCUMENTBYKEYREQUEST_H
