#include "abcapplogic.h"

AbcAppLogic::AbcAppLogic(QObject *parent) :
    QObject(parent)
{
}
void AbcAppLogic::init()
{
    //TODO: i guess like init the database n shit
}
void AbcAppLogic::handleRequestFromWtFrontEnd(AppLogicRequest *request)
{
}
void AbcAppLogic::handleResponseFromBankServer(BankServerActionRequestResponse *response)
{
}
