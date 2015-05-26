%RPC_AUTO_GENERATED_FILE_WARNING%
#include <Wt/WServer>
#include "%API_CALL_TO_REQUEST_BASE_NAME_TO_LOWERCASE%fromwt.h"

#include <boost/bind.hpp>

%API_CALL_TO_REQUEST_BASE_NAME%FromWt::%API_CALL_TO_REQUEST_BASE_NAME%FromWt(I%API_NAME% *%API_AS_VARIABLE_NAME%, const std::string &wtSessionId, boost::function<void (%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_VARNAMES%)> wApplicationCallback%API_CALL_ARG_TYPES_AND_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%)
    : I%API_CALL_TO_REQUEST_BASE_NAME%(%API_AS_VARIABLE_NAME%%API_CALL_ARG_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%)
    , m_WtSessionId(wtSessionId)
    , m_WApplicationCallback(wApplicationCallback)
{ }
void %API_CALL_TO_REQUEST_BASE_NAME%FromWt::respond(%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%)
{
    boost::bind(m_WApplicationCallback, %API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_TYPENAMES%);
    Wt::WServer::instance()->post(m_WtSessionId, boost::bind(m_WApplicationCallback, %API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_TYPENAMES%));
}
