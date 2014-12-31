#ifndef ABC2_COUCHBASE_AND_JSON_KEY_DEFINES_H
#define ABC2_COUCHBASE_AND_JSON_KEY_DEFINES_H

#include <cmath>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp> //bitcoin util

using namespace std;
using namespace boost::property_tree;

//sort of belongs here, but would more appropriately go into a bitcoin util file
typedef long long int SatoshiInt;
inline SatoshiInt satoshiStringToSatoshiInt(const std::string &satoshiString) { return boost::lexical_cast<SatoshiInt>(satoshiString); /* if changing to int64_t for some reason, need to cast 'through' double */ }
inline SatoshiInt jsonDoubleToSatoshiIntIncludingRounding(double inputDouble) { return (SatoshiInt)(inputDouble * 1e8 + (inputDouble < 0.0 ? -.5 : .5)); }
inline double satoshiIntToJsonDouble(SatoshiInt inputSatoshiInt) { return (double)inputSatoshiInt / 1e8; }
inline std::string satoshiIntToSatoshiString(SatoshiInt inputSatoshiInt) { return boost::lexical_cast<std::string>(inputSatoshiInt); /*char buf[20]; snprintf(buf, 20, "%d", inputSatoshiInt); std::string ret(buf, strlen(buf)); return ret;*/ }
//inline std::string jsonDoubleToJsonString(double inputDouble) { char buf[20]; snprintf(buf, 20, "%.8f", inputDouble); std::string ret(buf, strlen(buf)); return ret; }
//^shouldn't use because must convert to satoshi first to do proper rounding
inline std::string formatDoubleAs8decimalPlacesString(double inputDouble) { char buf[20]; snprintf(buf, 20, "%.8f", inputDouble); std::string ret(buf, strlen(buf)); return ret; }
inline std::string jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(double inputDouble) { double rounded = satoshiIntToJsonDouble(jsonDoubleToSatoshiIntIncludingRounding(inputDouble)); return formatDoubleAs8decimalPlacesString(rounded); }
inline std::string satoshiStringToJsonString(const std::string &inputSatoshiString) { std::string ret = jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(satoshiIntToJsonDouble(satoshiStringToSatoshiInt(inputSatoshiString))); return ret; }
inline double satoshiStringToJsonDouble(const std::string &inputSatoshiString) { SatoshiInt inputAsASatoshiInt = satoshiStringToSatoshiInt(inputSatoshiString); return satoshiIntToJsonDouble(inputAsASatoshiInt); }
inline SatoshiInt jsonStringToSatoshiInt(const std::string &inputJsonString) { return jsonDoubleToSatoshiIntIncludingRounding(boost::lexical_cast<double>(inputJsonString)); }
inline std::string satoshiIntToJsonString(SatoshiInt inputSatoshiInt) { return jsonDoubleToJsonStringAfterProperlyRoundingJsonDouble(satoshiIntToJsonDouble(inputSatoshiInt)); }

#define ABC2_WITHDRAWAL_FEE_PERCENT 3 /*if changing this, change below string*/
#define ABC2_WITHDRAWAL_FEE_PERCENT_STR "3"
inline double roundUpJsonDoubleToNearestSatoshi(double inputExactJsonDouble) { double withdrawalFeeInSatoshisAkaReadyForCeil = inputExactJsonDouble * 1e8; double withdrawalInSatoshisRoundedUp = ceil(withdrawalFeeInSatoshisAkaReadyForCeil); double withdrawalFeeRoundedUp = (double)withdrawalInSatoshisRoundedUp / 1e8; return withdrawalFeeRoundedUp; }
//this function requires a positive withdrawalAmount and withdrawalFeePercent. it always rounds up [to nearest satoshi], does not to "proper" rounding.
inline double withdrawalFeeForWithdrawalAmount(double withdrawalAmount) { double withdrawalFeeExact = withdrawalAmount * (ABC2_WITHDRAWAL_FEE_PERCENT / static_cast<double>(100)); return roundUpJsonDoubleToNearestSatoshi(withdrawalFeeExact); }

//Abc2 has a $1 USD (at time of writing it's ~.003 btc) minimum for various activities
#define ABC2_MIN_MIN_PRICE_OF_AD_CAMPAIGN_SLOT 0.00300000 /*if changing this, change below string too*/
#define ABC2_MIN_MIN_PRICE_OF_AD_CAMPAIGN_SLOT_STR "0.00300000"

#define ABC2_MIN_WITHDRAW_AMOUNT 0.00300000 /*if changing this, change below string too*/
#define ABC2_MIN_WITHDRAW_AMOUNT_STR "0.00300000"


#ifndef D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR
#define D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR "_"
#endif

class Abc2CouchbaseAndJsonKeyDefines //TODOoptional: move all global functions below into this class as static methods
{
private:
    Abc2CouchbaseAndJsonKeyDefines() { }
    Abc2CouchbaseAndJsonKeyDefines(const Abc2CouchbaseAndJsonKeyDefines &other) { (void)other; }
    virtual ~Abc2CouchbaseAndJsonKeyDefines() { }
public:
    //TODOoptional: static void createDefaultProfileDoc(ptree &emptyPtreeToPopulateWithDefaults); //etc
    static void createTransactionDoc(ptree &emptyPtreeToPopulate, const std::string &seller, const std::string &buyer, const std::string &amountInSatoshis);

    static bool profileIsLocked(const ptree &profile);
    //static bool profileIsLocked(const std::string &profile);

    static inline std::string propertyTreeToJsonString(ptree propertyTree)
    {
        std::ostringstream outputStringStream;
        write_json(outputStringStream, propertyTree, false);
        return outputStringStream.str();
    }
};

//campaign
#define COUCHBASE_AD_SPACE_CAMPAIGN_KEY_PREFIX "adSpaceCampaign" //if changing, change "all users with at least one ad space campaign" view (and possibly others). aka: don't change.
#define JSON_AD_SPACE_CAMPAIGN_MIN_PRICE "minPrice"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_LENGTH_HOURS "slotLengthHours" /*also used in current slot cache doc*/
//campaign-OPTIONAL-last-slot-filled
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED "lastSlotFilledAkaPurchased"
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_INDEX "slotIndex"
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_TIMESTAMP "purchaseTimestamp" //warning, this ,startTimestamp, and purchasePrice have duplicates below whose defines may have been accidentally been used in place of one another, so if you ever change either VALUE to something different, go through the source and make sure each one uses the correct define
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_START_TIMESTAMP "startTimestamp"
#define JSON_AD_SPACE_CAMPAIGN_LAST_SLOT_FILLED_PURCHASE_PRICE "purchasePrice"

//ex: adSpaceCampaignKey("d3fault", "0");
const std::string adSpaceCampaignKey(const std::string &usernameOfCampaignOwner, const std::string &campaignIndex);


//slot
#define COUCHBASE_AD_SPACE_CAMPAIGN_SLOT_KEY_PREFIX "adSpaceCampaignSlot"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_TIMESTAMP "purchaseTimestamp"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_START_TIMESTAMP "startTimestamp"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_PURCHASE_PRICE "purchasePrice"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_FILLED_WITH "slotFilledWith"

//ex: adSpaceCampaignSlotKey("d3fault", "0", "34");
const std::string adSpaceCampaignSlotKey(const std::string &usernameOfCampaignOwner, const std::string &campaignIndex, const std::string &slotIndex);


//slot filler
#define COUCHBASE_SLOT_FILLER_KEY_PREFIX "adSpaceSlotFiller"
#define JSON_SLOT_FILLER_USERNAME "username"
#define JSON_SLOT_FILLER_NICKNAME "nicknameB64"
#define JSON_SLOT_FILLER_HOVERTEXT "hoverTextB64"
#define JSON_SLOT_FILLER_URL "urlB64"
#define JSON_SLOT_FILLER_IMAGEB64 "adImageB64"
#define JSON_SLOT_FILLER_IMAGE_GUESSED_EXTENSION "adImageExt"

//ex: adSpaceSlotFillerKey("JimboKnives", "1");
const std::string adSpaceSlotFillerKey(const std::string &username, const std::string &slotFillerIndex);


//all slot fillers
#define COUCHBASE_ALL_SLOT_FILLERS_KEY_PREFIX "adSpaceAllSlotFillers"
#define JSON_ALL_SLOT_FILLERS_ADS_COUNT "adsCount"

//ex: adSpaceAllSlotFillersKey("JimboKnives");
const std::string adSpaceAllSlotFillersKey(const std::string &username);


//user account/profile
#define COUCHBASE_USER_ACCOUNT_KEY_PREFIX "user"
#define JSON_USER_ACCOUNT_BALANCE "balanceInSatoshis"
#define JSON_USER_ACCOUNT_PASSWORD_HASH "passwordHash"
#define JSON_USER_ACCOUNT_PASSWORD_SALT "passwordSalt"
//user account LOCKED [filling slot]
#define JSON_USER_ACCOUNT_SLOT_ATTEMPTING_TO_FILL "slotToAttemptToFillAkaPurchase"
#define JSON_USER_ACCOUNT_SLOT_TO_ATTEMPT_TO_FILL_IT_WITH "slotToAttemptToFillAkaPurchaseItWith"
//user account LOCKED [withdrawing funds]
#define JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS "lockedWithdrawingFunds"
//user account LOCKED [creditting transaction]
#define JSON_USER_ACCOUNT_LOCKED_CREDITTING_TRANSACTION "lockedCredittingTransaction"
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
#define COUCHBASE_TRANSACTION_KEY_PREFIX "tx" //if changing, change "all transactions with state of uncreditted" view (and possibly others). aka: don't change.
#define JSON_TRANSACTION_BUYER "buyer"
#define JSON_TRANSACTION_SELLER "seller"
#define JSON_TRANSACTION_AMOUNT "amountInSatoshis"

//transaction state (only used for [safely] creditting the seller for the amount)
#define JSON_TRANSACTION_STATE_KEY "txCredittedState" //if changing, change "all transactions with state of uncreditted" view (and possibly others). aka: don't change.
#define JSON_TRANSACTION_STATE_VALUE_UNCREDITTED "txUncreditted" //if changing, change "all transactions with state of uncreditted" view (and possibly others). aka: don't change.
#define JSON_TRANSACTION_STATE_VALUE_CREDITTED "txCreditted"

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


//campaign current index CACHE
#define COUCHBASE_AD_SPACE_CAMPAIGN_NEXT_AVAIABLE_INDEX_CACHE_KEY_PREFIX "adSpaceCampaignNextAvailableIndexCache"
#define JSON_AD_SPACE_CAMPAIGN_NEXT_AVAILABLE_INDEX_CACHE "nextAvailableCampaignIndex"

const std::string adSpaceCampaignIndexCacheKey(const std::string &usernameOfCampaignOwner);


//campaign current slot CACHE
#define COUCHBASE_AD_SPACE_CAMPAIGN_SLOT_CACHE_KEY_PREFIX "adSpaceCampaignCurrentSlotCache"
#define JSON_AD_SPACE_CAMPAIGN_SLOT_CACHE_CURRENT_SLOT "currentSlot"

const std::string adSpaceCampaignSlotCacheKey(const std::string &usernameOfCampaignOwner, const std::string &campaignIndex);


//withdraw funds requests
#define COUCHBASE_WITHDRAW_FUNDS_REQUESTS_PREFIX "withdrawFundsRequest"
#define JSON_WITHDRAW_FUNDS_REQUESTED_AMOUNT "desiredWithdrawAmountInSatoshisNotIncorporatingWithdrawalFee"
#define JSON_WITHDRAW_FUNDS_BITCOIN_PAYOUT_KEY "bitcoinKeyToWithdrawTo"
#define JSON_WITHDRAW_FUNDS_TXID "bitcoinTxId" //only appears when state is either processedButProfileNeedsDeductingAndUnlocking or processedDone
//withdraw funds requests state key
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY "withdrawFundsRequestState" //if changing, change "all withdrawal requests with state of unprocessed" view (and possibly others). aka: don't change
//withdraw funds requests state values (states)
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_UNPROCESSED "unprocessed" //if changing, change "all withdrawal requests with state of unprocessed" view (and possibly others). aka: don't change
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_INSUFFICIENTFUNDS "insufficientFunds_Done"
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSING "processing"
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_BITCOINDCOMMANDRETURNEDERROR "bitcoindCommandReturnedError_Done"
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDBUTPROFILENEEDSDEDUCTINGANDUNLOCKING "processedButProfileNeedsDeductingAndUnlocking"
#define JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDDONE "processed_Done"

#define JSON_WITHDRAW_FUNDS_REQUEST_BITCOINDERRORIFAPPLICABLE "bitcoindCommandErrorBase64" //only when bitcoindCommandReturnedError_Done

const std::string withdrawFundsRequestKey(const std::string &username, const std::string &withdrawFundsRequestIndex);


//withdraw funds requests current index (like 'CACHE')
#define COUCHBASE_WITHDRAW_FUNDS_REQUESTS_NEXT_AVAILABLE_INDEX_PREFIX "withdrawFundsRequestNextAvailableIndex"
#define JSON_WITHDRAW_FUNDS_REQUESTS_NEXT_AVAILABLE_INDEX "nextAvailableWithdrawFundsRequestIndex"



const std::string withdrawFundsRequestsNextAvailableIndexKey(const std::string &username);


//'get todays ad slot' json response
#define JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_KEY "error"
#define JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_NOERROR_VALUE "none"
#define JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_YESERROR_VALUE "unknown error"
#define JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR(customError) "{\""JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_KEY"\":\""customError"\"}"
#define JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_NO_AD_PLACEHOLDER  JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_CUSTOM_ERROR(JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_ERROR_YESERROR_VALUE)
#define JSON_TODAYS_AD_SPACE_SLOT_FILLER_RESPONSE_EXPIRATION_DATETIME "todaysAdSlotExpireDateTime"

#endif // ABC2_COUCHBASE_AND_JSON_KEY_DEFINES_H
