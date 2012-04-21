#include "abcbalancerequestpayout.h"

AbcBalanceRequestPayout::AbcBalanceRequestPayout(WContainerWidget *parent)
    : WContainerWidget(parent)
{
    addWidget(new WText("Hello Request Disbursement"));
}

const std::string AbcBalanceRequestPayout::ReadableText = "Request Disbursement";
const std::string AbcBalanceRequestPayout::PreferredInternalPath = "/balance/request-disbursement";

bool AbcBalanceRequestPayout::isInternalPath(const std::string &internalPath)
{
    if(internalPath == PreferredInternalPath)
        return true;
    if(internalPath == "/balance/disbursement")
        return true;
    if(internalPath == "/balance/payout")
        return true;
    if(internalPath == "/balance/requestdisbursement")
        return true;
    if(internalPath == "/balance/requestpayout")
        return true;
    return false;
}
bool AbcBalanceRequestPayout::requiresLogin()
{
    return true;
}
