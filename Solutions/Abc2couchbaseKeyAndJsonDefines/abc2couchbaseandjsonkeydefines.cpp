#include "abc2couchbaseandjsonkeydefines.h"

const std::string adSpaceCampaignKey(const std::string &usernameOfCampaignOwner, const std::string &campaignIndex)
{
    //ex: adSpaceCampaign_d3fault_0
    return COUCHBASE_AD_SPACE_CAMPAIGN_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + usernameOfCampaignOwner + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + campaignIndex;
}
const std::string adSpaceCampaignSlotKey(const std::string &usernameOfCampaignOwner, const std::string &campaignIndex, const std::string &slotIndex)
{
    //ex: adSpaceCampaign_d3fault_0_34
    return COUCHBASE_AD_SPACE_CAMPAIGN_SLOT_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + usernameOfCampaignOwner + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + campaignIndex + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + slotIndex;
}
const std::string adSpaceSlotFillerKey(const std::string &username, const std::string &slotFillerIndex)
{
    //ex: adSpaceSlotFiller_JimboKnives_1
    return COUCHBASE_SLOT_FILLER_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + username + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + slotFillerIndex;
}
const std::string adSpaceAllSlotFillersKey(const std::string &username)
{
    //ex: adSpaceAllSlotFillers_JimboKnives
    return COUCHBASE_ALL_SLOT_FILLERS_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + username;
}
const std::string userAccountKey(const std::string &username)
{
    //ex: user_JimboKnives
    return COUCHBASE_USER_ACCOUNT_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + username;
}
const std::string transactionKey(const std::string &campaignOwnerUsername, const std::string &campaignIndex, const std::string &slotIndex)
{
    //ex: tx_d3fault_0_1
    return COUCHBASE_TRANSACTION_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + campaignOwnerUsername + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + campaignIndex + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + slotIndex;
}
const std::string bitcoinKeySetCurrentPageKey(const std::string &setN)
{
    //ex: bitcoinKeySetCurrentPage_4
    return COUCHBASE_BITCOIN_KEY_SET_CURRENT_PAGE_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + setN;
}
const std::string bitcoinKeySetPageKey(const std::string &setN, const std::string &pageY)
{
    //ex: bitcoinKeySetPage_4_69
    return COUCHBASE_BITCOIN_KEY_SET_PAGE_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + setN + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + pageY;
}
const std::string hugeBitcoinKeyListCurrentPageKey() { return COUCHBASE_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY; } //derp
const std::string hugeBitcoinKeyListPageKey(const std::string &pageZ)
{
    //ex: hugeBitcoinKeyListPage_6
    return COUCHBASE_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + pageZ;
}
const std::string adSpaceCampaignIndexCacheKey(const std::string &usernameOfCampaignOwner)
{
    //ex: adSpaceCampaignNextAvailableIndexCache_d3fault
    return COUCHBASE_AD_SPACE_CAMPAIGN_NEXT_AVAIABLE_INDEX_CACHE_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + usernameOfCampaignOwner;
}
const std::string adSpaceCampaignSlotCacheKey(const std::string &usernameOfCampaignOwner, const std::string &campaignIndex)
{
    return COUCHBASE_AD_SPACE_CAMPAIGN_SLOT_CACHE_KEY_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + usernameOfCampaignOwner + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + campaignIndex;
}
const std::string withdrawFundsRequestKey(const std::string &username, const std::string &withdrawFundsRequestIndex)
{
    return COUCHBASE_WITHDRAW_FUNDS_REQUESTS_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + username + D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + withdrawFundsRequestIndex;
}
const std::string withdrawFundsRequestsNextAvailableIndexKey(const std::string &username)
{
    return COUCHBASE_WITHDRAW_FUNDS_REQUESTS_NEXT_AVAILABLE_INDEX_PREFIX
            D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR + username;
}
bool Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(const ptree &profile) //TODOreq: migrate all locked checks to this! TODOoptional: genericize the locking to be a single json key in the account
{
    string lockedCheck = profile.get<std::string>(JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL, "n");
    if(lockedCheck != "n")
    {
        return true;
    }

    lockedCheck = profile.get<std::string>(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS, "n");
    if(lockedCheck != "n")
    {
        return true;
    }

    return false;
}
/*
bool Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(const std::string &profile) -- if the string only needs to be checked for locked (we don't use the ptree for anythign else)
{
    TODO OPTIONAL
}
*/
