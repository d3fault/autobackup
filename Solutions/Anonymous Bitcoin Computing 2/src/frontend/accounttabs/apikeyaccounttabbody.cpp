#include "apikeyaccounttabbody.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <Wt/WText>
#include <Wt/WBreak>

using namespace boost::property_tree;
using namespace Wt;

#include "../anonymousbitcoincomputingwtgui.h"

//TODOoptional: the ability to request a new API key. If/when implementing, note that when encountering an invalid api key in a current slot cache doc, we can't consider invalid until we then check with the profile doc to see if it's been changed (either that or I have to update every current slot cache doc when requesting a new API key, but that could FAIL so has it's own problems). Unfortunately, this means 2 db hits whenever an invalid api key is presented
ApiKeyAccountTabBody::ApiKeyAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp)
{ }
void ApiKeyAccountTabBody::populateAndInitialize()
{
    new WText("Your API Key (this is only relevant for those who SELL ad space): ", this);
    new WBreak(this);

    //is it smarter to save this when we have the profile from the user logging in.... even though it'll rarely be requested? or to do a db hit whenever it is requested. i guess I answered my own question: get it when it's requested (it's a SLIGHT memory efficiency to not store it in memory [for EVERY user])
    m_AbcApp->getCouchbaseDocumentByKeyBegin(userAccountKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::GETPROFILEFORGETTINGAPIKEY;
}
void ApiKeyAccountTabBody::displayApiKey(const string &couchbaseDocument, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        return;
    }
    if(!lcbOpSuccess)
    {
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "TOTAL SYSTEM FAILURE: logged in user didn't have profile doc in ApiKeyAccountTabBody::displayApiKey -- '" << m_AbcApp->m_CurrentlyLoggedInUsername << endl;
        return;
    }

    //got profile doc

    ptree pt;
    std::istringstream is(couchbaseDocument);
    read_json(is, pt);

    new WText(pt.get<std::string>(JSON_USER_ACCOUNT_API_KEY), this);
}
