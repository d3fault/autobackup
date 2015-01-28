#include "apikeyaccounttabbody.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <Wt/WText>
#include <Wt/WBreak>

using namespace boost::property_tree;
using namespace Wt;

#include "../anonymousbitcoincomputingwtgui.h"
#include "abc2couchbaseandjsonkeydefines.h"

std::string ApiKeyAccountTabBody::s_ApiHttpsPort;
WResource* ApiKeyAccountTabBody::s_SimplePhpApiClientResource = NULL;

//TODOreq: api usage instructions. TODOoptional: zip-on-demand (or just js generation) for a given ad campaign (sufficiently random / not colliding with user's other ads in same html file (js-snippet) or even directory (php html generator))
//TODOoptional: the ability to request a new API key. If/when implementing, note that when encountering an invalid api key in a current slot cache doc, we can't consider invalid until we then check with the profile doc to see if it's been changed (either that or I have to update every current slot cache doc when requesting a new API key, but that could FAIL so has it's own problems). Unfortunately, this means 2 db hits whenever an invalid api key is presented
void ApiKeyAccountTabBody::setApiHttpsPort(const string &apiHttpsPort)
{
    s_ApiHttpsPort = apiHttpsPort;
}
void ApiKeyAccountTabBody::setSimplePhpApiClientResource(WResource *simplePhpApiClientResource)
{
    s_SimplePhpApiClientResource = simplePhpApiClientResource;
}
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

    const std::string &apiKey = pt.get<std::string>(JSON_USER_ACCOUNT_API_KEY);
    new WText(apiKey, this);


    //eh this should be in the constructor really, but I'm lazy...
    new WBreak(this);
    new WBreak(this);
    WAnchor *simplePhpApiClientAnchor = new WAnchor(WLink(WLink::Url, ABC_API_SIMPLE_PHP_CLIENT_URL), "Download: Simple PHP API Client", this);
    simplePhpApiClientAnchor->setTarget(TargetNewWindow);
    new WBreak(this);
    new WBreak(this);
    new WText("General Instructions: If you have no programming experience, the simplest way to get purchased ads showing up on your website is by placing an HTML snippet on your site (wherever you want the ad to be displayed), and additionally placing a PHP file in the same directory as that HTML file. If you don't have PHP installed on your server (or if you don't know if you have PHP installed on your server), contact your IT administrator for help." /*i love that empty statement, "contact your IT administrator". It's code for "you are a fucking noob, I'm not explaining this to you"*/, this); //TODOoptional: link to a my profile on some freelance site. DO-NOT-TODO-YET(I'd an hero): code a freelance site (use abc code as starting point tbh) <-- better, hire a freelancer to code a freelance site. it might even be smart to make the entire thing free/ad-based
    new WBreak(this);
    new WBreak(this);
    new WText("General Instructions (FOR PROGRAMMERS): Your website polls the API once every 5 minutes when the ad campaign has no purchases. When someone buys your ad, the relevant hovert/alt text, URL, and ad image (all 3 are base64 encoded) is supplied in a json document. Additionally, the ad's expiration unix timestamp is supplied as an integer (You do not query the API again until the ad expires). Lastly, there is a json field called \"error\". If there is no error, it's value is \"none\". If there is an error, it gives a description of the error. Note that when the ad space is not purchased there is an \"error\" of \"" ABC2_API_AD_SLOT_NOT_PURCHASED_ERROR "\", in which case you can display anything or nothing (I recommended you at least say 'click here to buy this ad space'). The 'Simple PHP API Client' provided above also doubles as a working exampke. It shows how a 'no.ad.placeholder.jpg'' image is used whenever \"error\" is not \"none\". You must cache/save the results of your API query and SERVE THE AD IMAGE TO YOUR END USERS YOURSELF. The 'Simple PHP API Client' demonstrates this caching technique. You may only query the API to get the details for a purchased ad " GetTodaysAdSlotServerClientConnection_MAX_API_REQUESTS_PER_AD_SLOT_DURATION_EXCLUDING_NO_AD_RESPONSES_OFC_STR " times. Querying more times than that will result in an error until the next ad begins (Note: when the ad slot is NOT currently purchased, you can query the API unlimited times, but only once every 5 minutes).", this);
    new WBreak(this);
    new WBreak(this);
    new WText("Example API Queries:", this);
    new WBreak(this);
    const std::string &apiSampleQuery = ABC2_API_GET_CURRENT_AD_ACTION_FULL_PATH_MACRO(m_AbcApp->m_CurrentlyLoggedInUsername, "0", apiKey);
    new WText("HTTPS: https://anonymousbitcoincomputing.com:" + s_ApiHttpsPort + apiSampleQuery, this);
    new WBreak(this);
    new WText("Tor Hidden Service: http://abcapijjqq7uvbpk.onion" + apiSampleQuery, this);
    new WBreak(this);
    new WText("Feel free to paste these example API queries into your browser right now to see what the JSON response looks like", this);
}
