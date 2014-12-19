#include "abc2withdrawrequestprocessor.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <libcouchbase/http.h>

#include <QScopedPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

//TO DOnereq: 3% fee. Implementing it is pretty easy, but I need to understand and decide on what to do with rounding errors. My "minimum withdrawal amount" might influence rounding (unsure). Basically I need to make sure that when I calculate how much I've earned by "3% of the offline wallet's total balance", that it REALLY IS EQUAL TO the 3% fee applied in this here app (so I'm not accidentally spending money that isn't mine). If I have to decide whether to round "toward me" or "toward the customers", I should round toward me.... because Superman 3. I shouldn't/won't lie about it, but still those satoshis (had:pennies) really do add up (and the customers don't miss them because they are insignificant when viewed individually)
//TO DOnereq: user profile balance deduction includes 3% fee, amount sent to bitcoind does not include 3% fee obviously (ALL MINE MUAHAHAHA). It almost sounds backwards written that way, so put it differently: the 3% fee is taken out (via math) before the bitcoind call
//TODOreq: I'm first going to code the "query view for all unprocessed" typical non-error case, BUT in order of program execution, the checking for previously failed "processing" (and possibly 'processedStillNeedToDeduct') states needs to come before that. I should definitely spit out that there are previosuly failed withdraw requests, and NOT continue onto querying view for all unprocessed states (until those fails are handled or ignored or whatever). I think even before THAT, I should check for profiles 'locked in withdraw' mode and do error recovery -- still that should never happen since I'd be watching the app run and see it's errors be spit out (and then manually fix)
//TODOreq: for now I'm just going to "process" the requests, but unless I have some kind of "this is how much I'm about to deduct: XXX. Does this make sense?" (and then I manually make sure that that much in bitcoins have been received)... followed by "press Y or N to continue or not"... then this app may as well be completely automated...
//TODOreq: abc2 and this app both need to account for the new locking mode, AND SO DOES the pessimistic recovery app. I have no idea what the implications of that are. OT: I'm scared as fuck to modify this locking code, and yet it's pretty much the only thing standing in the way of completion (aside from some polish etc)
//TODOoptional: another way to use this app without really changing any of the code, juse using it differently than how I originally plan, is to run it hourly/etc on a cron-job and to have it send me periodic emails telling me how much more funds the payout wallet needs. Basically, automating payouts completely will likely amount to tying together this app with cron, and having a trusted/openbsd/local 'amount verifier' (it receives those 'emails' saying how much is needed, and checks that the offline receive wallet actually received that much). With math I could even extrapolate how much is predicted to be needed on a daily basis, so I can just fund "one day in advance" (assuming I like the numbers. they aren't too big and i'm leik omgwtfz) and we'd have more or less (less, by est. 1 day) the same amount of security but now fully automated/instant payouts. -- TO START OFF, i will be running this app manually because there might be errors etc that require manual intervention
//TODOreq: at app startup we should ask the bitcoin payout wallet how much it has, and keep track of how much has been paid out. even if calculate-only mode is run before execute mode is run, we still might not have enough funds in the payout wallet by the time execute mode engages (more withdrawal requests filed since then). Whenever we encounter a withdrawal request that our payout wallet cannot process, we should skip that withdrawal request, and add that withdrawal request's amount to a rolling "adjusted calculation during execution", the grand total of which is spit out at the end of execution (and then you fund the payout wallet with that amount and run execute mode again). it's worth putting in it's own sentence: not having sufficient funds in the payout wallet (not to be confused with the user profile not having sufficient funds ofc) is not a fatal error -- still might be smarter (optimization-wise) to stop then and there, but eh since we'd then run calculate-only mode, it's the same (and perhaps MORE efficient) if we just continued and tallied up how much we didn't have enough to pay out
//TODOreq: I might need another state "invalid key", but that depends on how much sanitizing I do in the front-end. Really though that state would be ANY bitcoind error. Although actually maybe not, because I want to spit out the errors, exit the app, and leave the withdrawal request in INVALID state. Still as of now I am not sanitizing the bitcoin keys nearly enough to be confident that bitcoind SHOULDN'T give me errors.
//TODOreq: i think i already wrote this somewhere, but eh i need to update the 'islocked' logic in the pessimistic recoverer
Abc2WithdrawRequestProcessor::Abc2WithdrawRequestProcessor(QObject *parent)
    : QObject(parent)
    , ISynchronousLibCouchbaseUser()
    , m_NetworkAccessManager(new QNetworkAccessManager(this))
{
    connect(m_NetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkReplyFinished(QNetworkReply*)));
}
void Abc2WithdrawRequestProcessor::errorOutput(const string &errorString)
{
    QString errorQString = QString::fromStdString(errorString);
    emit e(errorQString);
}
void Abc2WithdrawRequestProcessor::viewQueryCompleteCallbackStatic(lcb_http_request_t request, lcb_t instance, const void *cookie, lcb_error_t error, const lcb_http_resp_t *resp)
{
    Q_UNUSED(request)
    Q_UNUSED(instance)
    (const_cast<Abc2WithdrawRequestProcessor*>(static_cast<const Abc2WithdrawRequestProcessor*>(cookie)))->viewQueryCompleteCallback(error, resp);
}
void Abc2WithdrawRequestProcessor::viewQueryCompleteCallback(lcb_error_t error, const lcb_http_resp_t *resp)
{
    emit o("viewQueryCompleteCallback called (good)");
    if(error != LCB_SUCCESS)
    {
        emit e("httpCompleteCallback has error");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //TO DOnereq: don't trust the view query. we still must get the doc (and we have to anyways to get the CAS in order to fuck with it. so that being said, i now know i can emit(null,null) (which gives us only the key) in the view that's a WIP-in-mah-brain) and check AGAIN then that the state is 'unprocessed'. hence the name of the variable:
    //QList<QString> listOfKeysWhereViewQuerySaidTheStateIsUnprocessedButHeyMaybeThatWasStaleInfo;

    QByteArray jsonResultByteArray(static_cast<const char*>(resp->v.v0.bytes), resp->v.v0.nbytes);
    QString jsonResultString(jsonResultByteArray);

    m_ViewQueryPropertyTree.clear();
    std::string jsonResultStdString = jsonResultString.toStdString();
    std::istringstream is(jsonResultStdString);
    read_json(is, m_ViewQueryPropertyTree);
    boost::optional<std::string> errorMaybe = m_ViewQueryPropertyTree.get_optional<std::string>("error");
    if(errorMaybe.is_initialized())
    {
        emit e("json had error set");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_ViewQueryHadError = false;
}
void Abc2WithdrawRequestProcessor::processNextWithdrawalRequestOrEmitFinishedIfNoMore()
{
    ++m_CurrentIndexIntoViewQueryResultsStringList;
    if(m_CurrentIndexIntoViewQueryResultsStringList < m_ViewQueryResultsStringListSize)
    {
        processWithdrawalRequest(m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate.at(m_CurrentIndexIntoViewQueryResultsStringList));
    }
    else
    {
        emit o("Withdrawal Request Processing Finished Successfully");
        emit withdrawalRequestProcessingFinished(true);
    }
}
void Abc2WithdrawRequestProcessor::processWithdrawalRequest(const QString &currentKeyToMaybeUnprocessedWithdrawalRequest)
{
    //"maybe" unprocessed because we don't trust the view query. it may have given us stale data

    //check state is really unprocessed
    m_CurrentKeyToMaybeUnprocessedWithdrawalRequest = currentKeyToMaybeUnprocessedWithdrawalRequest.toStdString();
    std::string getDescription = "the withdrawal request for verifying in state unprocessed for the first step of processing: " + m_CurrentKeyToMaybeUnprocessedWithdrawalRequest;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToMaybeUnprocessedWithdrawalRequest, getDescription))
    {
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_cas_t withdrawalRequestCas = m_LastGetCas;
    m_CurrentWithdrawalRequestPropertyTree.clear();
    std::istringstream withdrawalRequestIs(m_LastDocGetted);
    read_json(withdrawalRequestIs, m_CurrentWithdrawalRequestPropertyTree);
    const std::string &withdrawFundsRequestState = m_CurrentWithdrawalRequestPropertyTree.get<std::string>(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY);
    if(withdrawFundsRequestState != JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_UNPROCESSED)
    {
        //the view query was stale (despite stale=false) and the state was not in fact unprocessed... so we just return true and continue onto the next one ('processing' and other error states are handled at a different point (before app even does it's normal thing))
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return;
    }

    //state is 'unprocessed'

    //check profile not locked attempting to fill slot (or locked withdrawing)-- TODOreq: "slot attempting to fill" and "add funds" code needs to check that profile isn't locked in this new "withdraw funds" lock mode
    QStringList keyParts = currentKeyToMaybeUnprocessedWithdrawalRequest.split(D3FAULTS_COUCHBASE_SHARED_KEY_SEPARATOR);
    const QString &userRequestingWithdrawal = keyParts.at(1);
    if(m_UsersWithProfilesFoundLocked.contains(userRequestingWithdrawal))
    {
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return; //we already checked them and they have a locked profile, so we don't check any of their other withdrawal requests
    }
    m_CurrentUserRequestingWithdrawal = userRequestingWithdrawal.toStdString();
    getDescription = "the profile request to see if it's locked and then to lock it for user: " + m_CurrentUserRequestingWithdrawal;
    if(!couchbaseGetRequestWithExponentialBackoffRequiringSuccess(userAccountKey(m_CurrentUserRequestingWithdrawal), getDescription))
    {
        //error getting profile doc
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_cas_t userProfileCas = m_LastGetCas;
    m_CurrentUserProfilePropertyTree.clear();
    std::istringstream userProfileIs(m_LastDocGetted);
    read_json(userProfileIs, m_CurrentUserProfilePropertyTree);

    if(Abc2CouchbaseAndJsonKeyDefines::profileIsLocked(m_CurrentUserProfilePropertyTree))
    {
        m_UsersWithProfilesFoundLocked.append(userRequestingWithdrawal);
        emit o("user profile '" + userRequestingWithdrawal + "' is locked, continuing onto next. no more of this user's withdrawal requests will be processed and you must run this app again after fixing the locked account problem manually"); //TO DOne req: append to list of locked profiles, so we don't check again later for all their other withdrawal requests (especially since that would break ordering for processing of withdrawal requests. now changing from optimization to req)
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return; //just continue onto next for now. the above output is kind of like a "warning". it's not fatal, but we do need to intervene...
    }

    //TODOreq: verify sufficient funds and don't continue if not enough
    const std::string &userBalanceInSatoshisStr = m_CurrentUserProfilePropertyTree.get<std::string>(JSON_USER_ACCOUNT_BALANCE);
    m_CurrentUserBalanceInSatoshis = boost::lexical_cast<SatoshiInt>(userBalanceInSatoshisStr);

    std::string amountToWithdrawInSatoshisStr = m_CurrentWithdrawalRequestPropertyTree.get<std::string>(JSON_WITHDRAW_FUNDS_REQUESTED_AMOUNT);
    m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis = boost::lexical_cast<SatoshiInt>(amountToWithdrawInSatoshisStr);
    double desiredWithdrawRequestAmountAsJsonDouble = satoshiIntToJsonDouble(m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis);
    double withdrawalFeeJsonDouble = withdrawalFeeForWithdrawalAmount(desiredWithdrawRequestAmountAsJsonDouble);
    SatoshiInt withdrawalFeeInSatoshis = jsonDoubleToSatoshiIntIncludingRounding(withdrawalFeeJsonDouble); //the rounding, it does nothing (we already 'rounded up') ;-P!
    m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis = m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis + withdrawalFeeInSatoshis;

    if(m_CurrentUserBalanceInSatoshis < m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis)
    {
        //insufficient funds. not fatal error, they might have simply spent the funds on an ad slot filler after filing the withdrawal request. set the withdrawal request state to insiffucient funds done and return true;
        m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_INSUFFICIENTFUNDS);
        std::string insufficientFundsWithdrawalRequest = Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree);
        if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToMaybeUnprocessedWithdrawalRequest, insufficientFundsWithdrawalRequest, LCB_SET, withdrawalRequestCas, "setting withdrawal request '" + m_CurrentKeyToMaybeUnprocessedWithdrawalRequest + "' to insufficient funds"))
        {
            emit withdrawalRequestProcessingFinished(false);
            return; //on the other hand, failing to set the withdrawal funds request to insufficient funds IS fatal error
        }
        processNextWithdrawalRequestOrEmitFinishedIfNoMore();
        return;
    }

    //profile not locked and there are sufficient funds. lock profile.
    m_CurrentUserProfilePropertyTree.put(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS, m_CurrentKeyToMaybeUnprocessedWithdrawalRequest);
    std::ostringstream userProfileLockedForWithdrawingOs;
    write_json(userProfileLockedForWithdrawingOs, m_CurrentUserProfilePropertyTree, false);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(userAccountKey(m_CurrentUserRequestingWithdrawal), userProfileLockedForWithdrawingOs.str(), LCB_SET, userProfileCas, "locking user profile '" + m_CurrentUserRequestingWithdrawal + "' for withdrawal of funds request '" +  m_CurrentKeyToMaybeUnprocessedWithdrawalRequest + "'"))
    {
        //error locking profile doc. TODOreq: it may be a non-fatal error in that the cas swap simply failed (they are attempting to purchase a slot and the profile was locked? (or even they are adding funds or shit changing their password. tons of reasons this could fail) nothing wrong with that). right now we have 'return false', which indicates a fatal error. m_LastOpStatus is your friend here, and maybe i shouldn't use the "requiring success" method, idfk
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_CurrentUserProfileInLockedWithdrawingStateCas = m_LastSetCas;

    //cas swap set withdrawal request state to 'processing'
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSING);
    std::string withdrawalRequestInStateProcessingJson = Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToMaybeUnprocessedWithdrawalRequest, withdrawalRequestInStateProcessingJson, LCB_SET, withdrawalRequestCas, "setting withdrawal request '" + m_CurrentKeyToMaybeUnprocessedWithdrawalRequest + "' to processing"))
    {
        //error setting withdrawal request state to processing
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    m_CurrentWithdrawalRequestInProcessingStateCas = m_LastSetCas;

    //withdrawal request state is 'processing', so PROCESS it by issuing the payout command to bitcoin!
    QNetworkRequest request(QUrl("http://admin2:123@127.0.0.1:19011/")); //TODOreq: maybe qsettings 'profiles', but obviously some way to specify user/pw/ip/port (I lean against cli args [solely] because if I'm 'launched', I want to be able to show what's on my screen (but eh not a necessity initially, fuggit))
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    const std::string &keyToWithdrawTo = m_CurrentWithdrawalRequestPropertyTree.get<std::string>(JSON_WITHDRAW_FUNDS_BITCOIN_PAYOUT_KEY);
    QString keyToWIthdrawToQString = QString::fromStdString(keyToWithdrawTo);
    std::string desiredWithdrawAmountAsJsonString = satoshiIntToJsonString(m_CurrentWithdrawRequestDesiredAmountToWithdrawInSatoshis);
    QString desiredWithdrawAmountAsJsonQString = QString::fromStdString(desiredWithdrawAmountAsJsonString);
    m_CurrentBitcoinCommand = "{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"sendtoaddress\", \"params\": [\"" + keyToWIthdrawToQString + "\", " + desiredWithdrawAmountAsJsonQString + "] }";
    QByteArray payload = m_CurrentBitcoinCommand.toUtf8();
    m_NetworkAccessManager->post(request, payload); //TODOreq: separate bitcoin daemons on separate ports for payout/receive? or maybe use the "accounts" feature? i know for sure they need to be different wallets, so perhaps separate processes. (nope:the payout wallet daemon doesn't need to be running 24/7. in fact, this app can/should start/stop it on demand) since we want to stay in sync with the block chain, it does need to be running 24/7. there might be a way to make them share the blocks they receive to lessen bandwidth requirements, or maybe there is a way to do multiple wallets for one process, idfk
}
void Abc2WithdrawRequestProcessor::processWithdrawalRequests()
{
    if(!connectToCouchbase())
        return;
    emit o("Connected...");

    lcb_set_http_complete_callback(m_Couchbase, Abc2WithdrawRequestProcessor::viewQueryCompleteCallbackStatic);

    std::string viewPath = "_design/dev_AllWithdrawalRequestsWithStateOfUnprocessed/_view/AllWithdrawalRequestsWithStateOfUnprocessed?stale=false"; //TODOmb: perhaps if this app is run on the webserver, we should use some form of pagination so that all the requests don't have to be in ram. for now KISS. Another way to solve it is to run this app on a server other than the webserver
    lcb_http_cmd_t viewQueryCmd;
    viewQueryCmd.version = 0;
    viewQueryCmd.v.v0.path = viewPath.c_str();
    viewQueryCmd.v.v0.npath = viewPath.length();
    viewQueryCmd.v.v0.body = NULL;
    viewQueryCmd.v.v0.nbody = 0;
    viewQueryCmd.v.v0.method = LCB_HTTP_METHOD_GET;
    viewQueryCmd.v.v0.chunked = 0;
    viewQueryCmd.v.v0.content_type = "application/json";
    m_ViewQueryHadError = true; //prove to me that there wasn't an error ;-P
    lcb_error_t error = lcb_make_http_request(m_Couchbase, this, LCB_HTTP_TYPE_VIEW, &viewQueryCmd, NULL);
    if (error != LCB_SUCCESS)
    {
        emit e("Failed to query view " + QString::fromStdString(lcb_strerror(m_Couchbase, error)));
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_wait(m_Couchbase);
    if(m_ViewQueryHadError)
    {
        emit e("View query had error");
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //view successfully queried

    const ptree &rowsPt = m_ViewQueryPropertyTree.get_child("rows");
    m_UsersWithProfilesFoundLocked.clear();
    BOOST_FOREACH(const ptree::value_type &row, rowsPt)
    {
        //convert to QStringList, because i suck at using boost iterators and don't care to look it up. TODOoptimization: use an iterator or index to async iterate m_ViewQueryPropertyTree's rows DIRECTLY (instead of copying to qstringlist for same purpose)
        m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate.append(QString::fromStdString(row.second.get<std::string>("id")));
    }
    m_ViewQueryPropertyTree.clear(); //just an optimization, because we now have two copies of the list in memory lol

    m_CurrentIndexIntoViewQueryResultsStringList = -1;
    m_ViewQueryResultsStringListSize = m_ViewQueryResultsAsStringListBecauseEasierForMeToIterate.size();
    processNextWithdrawalRequestOrEmitFinishedIfNoMore();
}
void Abc2WithdrawRequestProcessor::handleNetworkReplyFinished(QNetworkReply *reply)
{
    QScopedPointer<QNetworkReply, QScopedPointerObjectDeleteLater<QNetworkReply> > replyDeleter(reply);
    Q_UNUSED(replyDeleter)
    if(reply->error() != QNetworkReply::NoError)
    {
        emit e("network reply has error: " + reply->errorString());
        emit e("the bitcoin request causing the error: " + m_CurrentBitcoinCommand);
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    QVariant responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if((!responseCode.isValid()) || (responseCode.toInt() != 200))
    {
        emit e("network reply did not return status code of 200. status code: " + QString::number(responseCode.toInt()));
        emit e("the bitcoin request causing the error: " + m_CurrentBitcoinCommand);
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    //check for json error
    QByteArray replyByteArray = reply->readAll();
    QString replyQString(replyByteArray);
    std::string replyStdString = replyQString.toStdString();
    std::istringstream is(replyStdString);
    ptree pt;
    read_json(is, pt);
    std::string jsonErrorMaybe = pt.get<std::string>("error");
    if(jsonErrorMaybe != "null")
    {
        emit e("network reply had json error set: " + QString::fromStdString(jsonErrorMaybe));
        emit e("the bitcoin request causing the error: " + m_CurrentBitcoinCommand);
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //the bitcoin sendtoaddress command response had no errors

    //TO DOne optional: the ptree in scope contains a txid that we could store in the withdrawal request (when putting in the 'done' state) for book keeping and such. hell, i might [legally] have to! TODOreq: figure out if it's a legal requirement (i'm still curious even though i already implemented it). It might also help in error recovery, but I'm unsure
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_TXID, pt.get<std::string>("result"));

    //cas swap set withdrawal request state to "processed, profile needs balance deducting/unlocking"
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDBUTPROFILENEEDSDEDUCTINGANDUNLOCKING);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToMaybeUnprocessedWithdrawalRequest, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree), LCB_SET, m_CurrentWithdrawalRequestInProcessingStateCas, "setting withdrawal request '" + m_CurrentKeyToMaybeUnprocessedWithdrawalRequest + "' to processedButProfileNeedsDeductingAndUnlocking"))
    {
        //error setting withdrawal request state to processedButProfileNeedsDeductingAndUnlocking
        emit withdrawalRequestProcessingFinished(false);
        return;
    }
    lcb_cas_t withdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas = m_LastSetCas;

    //deduct amount from profile balance and cas swap unlock it
    SatoshiInt originalBalance = m_CurrentUserBalanceInSatoshis;
    m_CurrentUserBalanceInSatoshis -= m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis;
    m_CurrentUserProfilePropertyTree.put(JSON_USER_ACCOUNT_BALANCE, m_CurrentUserBalanceInSatoshis);
    m_CurrentUserProfilePropertyTree.erase(JSON_USER_ACCOUNT_LOCKED_WITHDRAWING_FUNDS);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(userAccountKey(m_CurrentUserRequestingWithdrawal), Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentUserProfilePropertyTree), LCB_SET, m_CurrentUserProfileInLockedWithdrawingStateCas, "debitting+unlocking user profile '" + m_CurrentUserRequestingWithdrawal + "' with amount: (OLD: " + satoshiIntToJsonString(originalBalance) + ", NEW: " + satoshiIntToJsonString(m_CurrentUserBalanceInSatoshis) + ", DIFF: -" + satoshiIntToJsonString(m_CurrentWithdrawRequestTotalAmountToWithdrawIncludingWithdrawalFeeInSatoshis) + ")"))
    {
        //error debitting+unlocking user profile
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //set withdrawal request state to "processed and done"
    m_CurrentWithdrawalRequestPropertyTree.put(JSON_WITHDRAW_FUNDS_REQUEST_STATE_KEY, JSON_WITHDRAW_FUNDS_REQUEST_STATE_VALUE_PROCESSEDDONE);
    if(!couchbaseStoreRequestWithExponentialBackoffRequiringSuccess(m_CurrentKeyToMaybeUnprocessedWithdrawalRequest, Abc2CouchbaseAndJsonKeyDefines::propertyTreeToJsonString(m_CurrentWithdrawalRequestPropertyTree), LCB_SET, withdrawRequestInProcessedButProfileNeedsDeductingAndUnlockingCas, "setting withdrawal request '" + m_CurrentKeyToMaybeUnprocessedWithdrawalRequest + "' to processedDone"))
    {
        //error setting withdrawal request state to processedDone
        emit withdrawalRequestProcessingFinished(false);
        return;
    }

    //onto the next withdrawal request. this used to be a while loop (easier to read/understand), but QNetworkAccessManager requires async guh
    processNextWithdrawalRequestOrEmitFinishedIfNoMore();
}
