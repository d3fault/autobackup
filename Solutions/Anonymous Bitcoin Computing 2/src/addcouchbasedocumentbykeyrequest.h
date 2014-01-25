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
    enum LcbStoreMode
    {
        AddMode,
        StoreModeNoCas,
        StoreModeWithCas
    };
    //save constructor no cas overload
    AddCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, LcbStoreMode lcbStoreMode = AddMode)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput), LcbModeIsAdd(lcbStoreMode == AddMode ? true : false), HasCAS(lcbStoreMode == StoreModeWithCas ? true : false)
    { }
    //save constructor
    AddCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, LcbStoreMode lcbStoreMode, u_int64_t cas)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput), LcbModeIsAdd(false), HasCAS(true), CAS(cas)
    { }
    //load constructor
    AddCouchbaseDocumentByKeyRequest()
        : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), LcbModeIsAdd(true), HasCAS(false)
    { }

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseAddKeyInput;
    std::string CouchbaseAddDocumentInput;
    bool LcbModeIsAdd; //LCB_ADD vs. LCB_SET
    bool HasCAS; //whether or not the following CAS is valid or not
    u_int64_t CAS;

    static inline void respond(AddCouchbaseDocumentByKeyRequest *originalRequest)
    {
        if(!originalRequest->LcbModeIsAdd)
        {
            Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1), originalRequest->CouchbaseAddKeyInput));
            return;
        }
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
        ar & LcbModeIsAdd;
        ar & HasCAS;
        if(HasCAS)
        {
            ar & CAS;
        }
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
        ar & LcbModeIsAdd;
        ar & HasCAS;
        if(HasCAS)
        {
            ar & CAS;
        }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // ADDCOUCHBASEDOCUMENTBYKEYREQUEST_H
