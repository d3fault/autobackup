%RPC_AUTO_GENERATED_FILE_WARNING%
#ifndef %API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUESTFROMQT_H
#define %API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUESTFROMQT_H

%API_CALL_REQUEST_INTERFACE_HEADER_INCLUDE%

class %API_CALL_TO_REQUEST_BASE_NAME%FromQt : public I%API_CALL_TO_REQUEST_BASE_NAME%
{
    Q_OBJECT
public:
    %API_CALL_TO_REQUEST_BASE_NAME%FromQt(I%API_NAME% *%API_AS_VARIABLE_NAME%, %API_NAME%Session %API_AS_VARIABLE_NAME%Session, QObject *objectCallback, const char *slotCallback%API_CALL_ARG_TYPES_AND_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%);
    void respond(bool internalError, bool %API_CALL_SLOT_NAME%Success%COMMASPACE_IF_RESPONSE_HAS_ANY_ARGS%%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%);
signals:
    void %API_CALL_SLOT_NAME%ResponseRequested(bool internalError, bool %API_CALL_SLOT_NAME%Success%COMMASPACE_IF_RESPONSE_HAS_ANY_ARGS%%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%);
};

#endif // %API_NAME_UPPERCASE%%API_CALL_SLOT_NAME_TO_UPPER%REQUESTFROMQT_H
