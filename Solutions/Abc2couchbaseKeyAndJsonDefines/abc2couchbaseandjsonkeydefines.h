#ifndef ABC2_COUCHBASE_AND_JSON_KEY_DEFINES_H
#define ABC2_COUCHBASE_AND_JSON_KEY_DEFINES_H

#include <string>

#define ABC_COUCHBASE_KEY_SEPARATOR "_"

//doesn't belong in this class, but /lazy:
#define ABC_COUCHBASE_LCB_ERROR_TYPE_IS_ELIGIBLE_FOR_EXPONENTIAL_BACKOFF(errorVarName) errorVarName == LCB_EBUSY || errorVarName == LCB_ETMPFAIL || errorVarName == LCB_CLIENT_ETMPFAIL

//campaign
#define COUCHBASE_AD_SPACE_CAMPAIGN_KEY_PREFIX "adSpaceCampaign"
#define JSON_AD_SPACE_CAMPAIGN_MIN_PRICE "minPrice"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS "slotLengthHours"
//campaign-OPTIONAL-last-slot-filled
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED "lastSlotFilledAkaPurchased"
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX "slotIndex"
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_TIMESTAMP "purchaseTimestamp" //warning, this ,startTimestamp, and purchasePrice have duplicates below whose defines may have been accidentally been used in place of one another, so if you ever change either VALUE to something different, go through the source and make sure each one uses the correct define
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_START_TIMESTAMP "startTimestamp"
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_PRICE "purchasePrice"

//ex: adSpaceCampaignKey("d3fault", "0");
const std::string adSpaceCampaignKey(const std::string &username, const std::string &campaignIndex);


//slot
#define COUCHBASE_AD_SPACE_CAMPAIGN_SLOT_KEY_PREFIX "adSpaceCampaignSlot"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_TIMESTAMP "purchaseTimestamp"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_START_TIMESTAMP "startTimestamp"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_PRICE "purchasePrice"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH "slotFilledWith"

//ex: adSpaceCampaignSlotKey("d3fault", "0", "34");
const std::string adSpaceCampaignSlotKey(const std::string &username, const std::string &campaignIndex, const std::string &slotIndex);


//slot filler
#define COUCHBASE_SLOT_FILLER_KEY_PREFIX "adSpaceSlotFiller"
#define JSON_SLOT_FILLER_USERNAME "username"
#define JSON_SLOT_FILLER_NICKNAME "nickname"
#define JSON_SLOT_FILLER_HOVERTEXT "hoverText"
#define JSON_SLOT_FILLER_URL "url"
#define JSON_SLOT_FILLER_IMAGEB64 "adImageB64"
#define JSON_SLOT_FILLER_IMAGE_GUESSED_EXTENSION "adImageExt"

//ex: adSpaceSlotFillerKey("JimboKnives", "1");
const std::string adSpaceSlotFillerKey(const std::string &username, const std::string &slotFillerIndex);


//all slot fillers
#define COUCHBASE_ALL_SLOT_FILLERS_KEY_PREFIX "adSpaceAllSlotFillers"
#define JSON_ALL_SLOT_FILLERS_ADS_COUNT "adsCount"

//ex: adSpaceAllSlotFillersKey("JimboKnives");
const std::string adSpaceAllSlotFillersKey(const std::string &username);


//user account
#define COUCHBASE_USER_ACCOUNT_KEY_PREFIX "user"
#define JSON_USER_ACCOUNT_BALANCE "balance"
#define JSON_USER_ACCOUNT_PASSWORD_HASH "passwordHash"
#define JSON_USER_ACCOUNT_PASSWORD_SALT "passwordSalt"
//user account LOCKED [filling slot]
#define JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL "slotToAttemptToFillAkaPurchase"
#define JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH "slotToAttemptToFillAkaPurchaseItWith"
//bitcoin state
#define JSON_USER_ACCOUNT_BITCOIN_STATE "bitcoinState"
#define JSON_USER_ACCOUNT_BITCOIN_STATE_NO_KEY "NoKey"
#define JSON_USER_ACCOUNT_BITCOIN_STATE_GETTING_KEY "GettingKey"
#define JSON_USER_ACCOUNT_BITCOIN_STATE_HAVE_KEY "HaveKey"
//optional bitcoin state
#define JSON_USER_ACCOUNT_BITCOIN_STATE_DATA "bitcoinStateData" //SetN (GettingKey) || BitcoinKey (HaveKey)
#define JSON_USER_ACCOUNT_BITCOIN_SET_PAGE "bitcoinSetPage" //PageY (GettingKey)
#define JSON_USER_ACCOUNT_BITCOIN_GET_KEY_UUID "uuidPerKey" //per-key-request-uuid (GettingKey)

//ex: userAccountKey("JimboKnives");
const std::string userAccountKey(const std::string &username);


//transaction
#define COUCHBASE_TRANSACTION_KEY_PREFIX "tx"
#define JSON_TRANSACTION_BUYER "buyer"
#define JSON_TRANSACTION_SELLER "seller"
#define JSON_TRANSACTION_AMOUNT "amount"

//ex: transactionKey("d3fault", "0", "1");
const std::string transactionKey(const std::string &campaignOwnerUsername, const std::string &campaignIndex, const std::string &slotIndex);


//current page of bitcoin key set
#define COUCHBASE_BITCOIN_KEY_SET_CURRENT_PAGE_KEY_PREFIX "bitcoinKeySetCurrentPage"
#define JSON_BITCOIN_KEY_SET_CURRENT_PAGE "currentPage"
//current page of bitcoin key set LOCKED [filling next page]
#define JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE "fillingNextBitcoinKeySetPage"
#define JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_UUID_PER_FILL "uuidPerFill"
#define JSON_BITCOIN_KEY_SET_CURRENT_PAGE_LOCKED_FILLING_NEXT_PAGE_START_LOOKING_ON_HUGEBITCOINKEYLIST_PAGE "hugeBitcoinKeyListStartingPage"

//ex: bitcoinKeySetCurrentPageKey("4");
const std::string bitcoinKeySetCurrentPageKey(const std::string &setN);


//bitcoin key set page
#define COUCHBASE_BITCOIN_KEY_SET_PAGE_KEY_PREFIX "bitcoinKeySetPage"
#define JSON_BITCOIN_KEY_SET_KEY_PREFIX "key"
//bitcoin key set (optional claimed key indication)
#define JSON_BITCOIN_KEY_SET_CLAIMED_UUID_SUFFIX "claimedUuid"
#define JSON_BITCOIN_KEY_SET_CLAIMED_USERNAME_SUFFIX "claimedUsername"

//ex: bitcoinKeySetPageKey("4", "69");
const std::string bitcoinKeySetPageKey(const std::string &setN, const std::string &pageY);


//current page of huge bitcoin key list
#define COUCHBASE_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY "hugeBitcoinKeyListCurrentPage"
#define JSON_HUGE_BITCOIN_KEY_LIST_CURRENT_PAGE_KEY "currentPage"

const std::string hugeBitcoinKeyListCurrentPageKey();


//huge bitcoin key list page
#define COUCHBASE_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_PREFIX "hugeBitcoinKeyListPage"
#define JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_PREFIX "keyRange"
//huge bitcoin key list page (optional claimed key range indication)
#define JSON_HUGE_BITCOIN_KEY_LIST_PAGE_KEY_RANGE_CLAIMED_UUID_SUFFIX "keyRangeClaimedUuid"

//ex: hugeBitcoinKeyListPageKey("6");
const std::string hugeBitcoinKeyListPageKey(const std::string &pageZ);

#endif // ABC2_COUCHBASE_AND_JSON_KEY_DEFINES_H
