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
    enum LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum
    {
        AddMode,
        StoreNoCasMode,
        StoreWithCasMode
    };
    enum WhatToDoWithOutputCasEnum
    {
        DiscardOuputCasMode,
        SaveOutputCasMode
    };
    //save constructor no cas overload
    AddCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode = AddMode, WhatToDoWithOutputCasEnum whatToDoWithOutputCas = DiscardOuputCasMode)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput), LcbStoreModeIsAdd(lcbStoreMode == AddMode ? true : false), HasCasInput(lcbStoreMode == StoreWithCasMode ? true : false), SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
    { }
    //save constructor cas overload
    AddCouchbaseDocumentByKeyRequest(std::string wtSessionId, AnonymousBitcoinComputingWtGUI *pointerToAnonymousBitcoinComputingWtGUI, std::string CouchbaseAddKeyInput, std::string couchbaseAddDocumentInput, LcbStoreMode_AndWhetherOrNotThereIsInputCasEnum lcbStoreMode, u_int64_t cas, WhatToDoWithOutputCasEnum whatToDoWithOutputCas = DiscardOuputCasMode)
        : WtSessionId(wtSessionId), AnonymousBitcoinComputingWtGUIPointerForCallback(pointerToAnonymousBitcoinComputingWtGUI), CouchbaseAddKeyInput(CouchbaseAddKeyInput), CouchbaseAddDocumentInput(couchbaseAddDocumentInput), LcbStoreModeIsAdd(false), HasCasInput(true), CasInput(cas), SaveCasOutput(whatToDoWithOutputCas == SaveOutputCasMode ? true : false)
    { }
    //load constructor
    AddCouchbaseDocumentByKeyRequest()
        : AnonymousBitcoinComputingWtGUIPointerForCallback(NULL), LcbStoreModeIsAdd(true), HasCasInput(false), SaveCasOutput(false)
    { }

    std::string WtSessionId;
    AnonymousBitcoinComputingWtGUI *AnonymousBitcoinComputingWtGUIPointerForCallback;
    static const unsigned char LengthOfAddressToAnonymousBitcoinComputingWtGUI = sizeof(AnonymousBitcoinComputingWtGUI*) + 1;
    std::string CouchbaseAddKeyInput;
    std::string CouchbaseAddDocumentInput;
    bool LcbStoreModeIsAdd; //LCB_ADD vs. LCB_SET
    bool HasCasInput; //whether or not the following CAS is valid or not
    u_int64_t CasInput;
    bool SaveCasOutput;

    static inline void respond(AddCouchbaseDocumentByKeyRequest *originalRequest)
    {
        if(!originalRequest->LcbStoreModeIsAdd)
        {
            Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1), originalRequest->CouchbaseAddKeyInput));
            return;
        }
        Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::addCouchbaseDocumentByKeyFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1), originalRequest->CouchbaseAddKeyInput));
    }
    static inline void respondWithCas(AddCouchbaseDocumentByKeyRequest *originalRequest, u_int64_t casOutput)
    {
        Wt::WServer::instance()->post(originalRequest->WtSessionId, boost::bind(boost::bind(&AnonymousBitcoinComputingWtGUI::setCouchbaseDocumentByKeyWithInputCasSavingOutputCasFinished, originalRequest->AnonymousBitcoinComputingWtGUIPointerForCallback, _1, _2), originalRequest->CouchbaseAddKeyInput, casOutput));
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
        ar & SaveCasOutput;
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
        ar & SaveCasOutput;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif // ADDCOUCHBASEDOCUMENTBYKEYREQUEST_H
