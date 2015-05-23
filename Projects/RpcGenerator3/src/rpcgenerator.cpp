#include "rpcgenerator.h"

#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QDir>

#define EEEEEEEEEE_RETURN_RpcGenerator(errorMessage, returnWhat) \
{ \
    emit e(errorMessage); \
    emit rpcGenerated(false); \
    return returnWhat; \
}

#define EEEEEEEEEE_RpcGenerator(errorMessage) EEEEEEEEEE_RETURN_RpcGenerator(errorMessage, )

#define NoApiCallArgs QList<ApiCallArg>()

//this is hacked together as fast as possible. ugly and lots of places elegance and efficiencies can be improved (dgaf)
//TODOmb: generate matching .pri file
RpcGenerator::RpcGenerator(QObject *parent)
    : QObject(parent)
{ }
class GeneratableApis //temporary solution until I can use (had:parse) some user input
{
public:
    GeneratableApis()
    {
        generateCleanRoomApi();
        generateDocumentTimelineApi();
    }
    Api apiByName(QString apiName, bool *apiFoundOptionalErrorResultCode = 0)
    {
        QHash<QString, Api>::const_iterator apiMaybe = m_Apis.find(apiName);
        if(apiMaybe == m_Apis.end())
        {
            if(apiFoundOptionalErrorResultCode)
                *apiFoundOptionalErrorResultCode = false;
            Api api;
            return api;
        }
        if(apiFoundOptionalErrorResultCode)
            *apiFoundOptionalErrorResultCode = true;
        return *apiMaybe;
    }
private:
    QHash<QString, Api> m_Apis;

    void generateCleanRoomApi()
    {
        //QSharedPointer<Api> api(QSharedPointer<Api>(new QSharedPointer)("CleanRoom"));
        Api api("CleanRoom");
        api.createApiCall("frontPageDefaultView", NoApiCallArgs, QList<ApiCallArg>() << ApiCallArg("QStringList", "frontPageDocs"));
        m_Apis.insert(api.ApiName, api);
    }
    void generateDocumentTimelineApi()
    {
        Api api("DocumentTimeline");
        api.createApiCall("getLatestDocuments", NoApiCallArgs, QList<ApiCallArg>() << ApiCallArg("QStringList", "latestDocuments"));
        //apicreateApiCall("registerAttemptVideoSubmitted", QList<ApiCallArg>() << ApiCallArg("QString", "desiredUsername") << ApiCallArg("QByteArray", "registrationAttemptSubmissionVideo"), QList<ApiCallArg>() << ApiCallArg("bool", "regi"));
        api.createApiCall("submitRegistrationAttemptVideo", QList<ApiCallArg>() << ApiCallArg("QString", "desiredUsername") << ApiCallArg("QByteArray", "registrationAttemptSubmissionVideo"), QList<ApiCallArg>() << ApiCallArg("bool", "registrationAttemptVideoSubmissionSuccessful"));
        api.createApiCall("login", QList<ApiCallArg>() << ApiCallArg("QString", "username") << ApiCallArg("QString", "password"), QList<ApiCallArg>() << ApiCallArg("bool", "loginSuccessful"));
        //api.createApiCall("post", QList<ApiCallArg>() << ApiCallArg("QByteArray", "document"), QList<ApiCallArg>() << ApiCallArg("bool", "postedSuccessfully") << ApiCallArg("QByteArray", "documentKey"));
        //api.createApiCall("logout", NoApiCallArgs, QList<ApiCallArg>() << ApiCallArg("bool", "success"));
        m_Apis.insert(api.ApiName, api);
    }
};
void RpcGenerator::generateRpc()
{
    QTemporaryDir tempDir;
    if(!tempDir.isValid())
        EEEEEEEEEE_RpcGenerator("unable to get temp dir")
    QString outputPath = tempDir.path();
    GeneratableApis apis;
    bool foundApi = false;


    Api api = apis.apiByName("DocumentTimeline", &foundApi);


    if(!foundApi)
        EEEEEEEEEE_RpcGenerator("could not find api")
    if(!generateRpcActual(api, outputPath))
        EEEEEEEEEE_RpcGenerator("failed to generate")
    tempDir.setAutoRemove(false);
    emit o("successfully generated rpc to: " + outputPath);
    emit rpcGenerated(true, outputPath);
}
QString RpcGenerator::frontLetterToLower(const QString &stringInput)
{
    QString ret = stringInput;
    if(ret.isEmpty())
        return ret;
    QString firstLetter = ret.at(0);
    ret.remove(0, 1);
    ret.insert(0, firstLetter.toLower());
    return ret;
}
QString RpcGenerator::frontLetterToUpper(const QString &stringInput)
{
    QString ret = stringInput;
    if(ret.isEmpty())
        return ret;
    QString firstLetter = ret.at(0);
    ret.remove(0, 1);
    ret.insert(0, firstLetter.toUpper());
    return ret;
}
QString RpcGenerator::classHeaderFileName(QString apiName)
{
    return apiName.toLower() + ".h";
}
QString RpcGenerator::classSourceFileName(QString apiName)
{
    return apiName.toLower() + ".cpp";
}
QString RpcGenerator::apiCallToRequestBaseName(ApiCall apiCall)
{
    return apiCall.ParentApi.ApiName + frontLetterToUpper(apiCall.ApiCallSlotName) + "Request";
}
QString RpcGenerator::apiCallToRequestInterfaceTypeName(ApiCall apiCall)
{
    return "I" + apiCallToRequestBaseName(apiCall);
}
QString RpcGenerator::apiCallToRequestInterfaceHeaderInclude(ApiCall apiCall)
{
    return "#include \"" + apiCallToRequestInterfaceTypeName(apiCall).toLower() + ".h\"";
}
QString RpcGenerator::apiCallToForwardDefinitionRawCpp(ApiCall apiCall)
{
    return "class " + apiCallToRequestInterfaceTypeName(apiCall) + ";";
}
QString RpcGenerator::apiCallArgNamesToCommaSeparatedList(ApiCall apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter, bool emitTypes, bool emitNames)
{
    QStringList apiCallArgsRawCpp;
    if(makeRequestPointerFirstParameter)
        apiCallArgsRawCpp.append(apiCallToRequestInterfaceTypeName(apiCall) + " *request");
    Q_FOREACH(ApiCallArg apiCallArg, (requestIfTrue ? apiCall.RequestArgs : apiCall.ResponseArgs))
    {
        apiCallArgsRawCpp.append(apiCallArgToCpp(&apiCallArg, emitTypes, emitNames));
    }
    return apiCallArgsRawCpp.join(", ");
}
QString RpcGenerator::apiCallToMethodCppSignature(ApiCall apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter)
{
    QString ret(apiCall.ApiCallSlotName + QString(requestIfTrue ? "" : "Finished"));
    QString apiCallArgsAsCommaSeparatedList = apiCallArgNamesToCommaSeparatedList(apiCall, requestIfTrue, makeRequestPointerFirstParameter);
    ret.append("(" + apiCallArgsAsCommaSeparatedList + ")");
    return ret;
}
QString RpcGenerator::apiCallToRawCppDeclaration(ApiCall apiCall, bool requestIfTrue)
{
    QString ret("    void " + apiCallToMethodCppSignature(apiCall, requestIfTrue) + ";");
    return ret;
}
QString RpcGenerator::apiCallArgToCpp(ApiCallArg apiCallArg, bool emitTypes, bool emitNames)
{
    QString ret;
    if(!emitTypes)
    {
        ret.append(apiCallArg->ApiCallArgType);
    }
    if((!emitTypes) && (!emitNames))
    {
        if(!apiCallArg->ApiCallArgType.endsWith(" *")) //we want points to be next to the variable name, unless the pointer is next to the type
            ret.append(" ");
    }
    if(!emitNames)
    {
        ret.append(apiCallArg->ApiCallArgName);
    }
    return ret;
}
QString RpcGenerator::apiCallToApiDefinitionRawCpp(ApiCall apiCall, bool requestIfTrue)
{
    QString methodSignatureForDefinition = apiCall.ParentApi.ApiName + "::" + apiCallToMethodCppSignature(apiCall, requestIfTrue);
    QString ret("void " + methodSignatureForDefinition + "\n{\n    ");
    if(requestIfTrue)
    {
        ret.append("qFatal(\"" + methodSignatureForDefinition + " not yet implemented\");");
    }
    else
    {
        ret.append("request->respond(" + apiCallArgNamesToCommaSeparatedList(apiCall, false, false, true, false) + ");");
    }
    ret.append("\n}");
    return ret;
}
QString RpcGenerator::apiCallToApiCallRequestMethodDeclarationInSessionHeader(ApiCall apiCall, const QString &boostOrQtCallbackArgs, bool includeApiNamePrefix /*whether or not to put 'Class::' in front ish*/ )
{
    QString ret;
    QString methodExcludingArgs = "void " + (includeApiNamePrefix ? (apiCall.ParentApi.ApiName + "::") : "") + "requestNew" + apiCall.ParentApi.ApiName + frontLetterToUpper(apiCall.ApiCallSlotName) + "(";
    QString maybeCommaIfApiCallHasArgs = (apiCall.RequestArgs.isEmpty() ? "" : ", ");
    ret.append(methodExcludingArgs + boostOrQtCallbackArgs + maybeCommaIfApiCallHasArgs + apiCallArgNamesToCommaSeparatedList(apiCall, true, false)); //TODOmb: considering putting the api call args in front of the [non-changing] callback variable name, BUT then there would be problems in the future if/when I implement args with default values (however, move overloads solves this (when in rome (and in this case, it's "when you're a code generator"... more overloads are CHEAP for a code generator)))
    ret.append(")");
    return ret;
}
QString RpcGenerator::apiCallToApiCallRequestMethodDefinitionInSessionSource(ApiCall apiCall, bool trueIfQt_falseIfWt)
{
    QString ret;
    QString requseterDomainSpecificRequestType = apiCallToRequestBaseName(apiCall) + (trueIfQt_falseIfWt ? "FromQt" : "FromWt");
    QString commaMaybe = apiCall.RequestArgs.isEmpty() ? "" : ", ";
    QString requesterDomainSpecificCallbackArgsWithTypes = (trueIfQt_falseIfWt ? "QObject *objectToCallbackTo, const char *callbackSlot" : "const std::string &wtSessionId, boost::function<void (" + apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false, true) + ")> wApplicationCallback");
    QString requesterDomainSpecificCallbackArgsWithoutTypes = (trueIfQt_falseIfWt ? "objectToCallbackTo, callbackSlot" : "wtSessionId, wApplicationCallback");
    ret.append(apiCallToApiCallRequestMethodDeclarationInSessionHeader(apiCall, requesterDomainSpecificCallbackArgsWithTypes, true) + "\n{\n    " + requseterDomainSpecificRequestType + " *request = new " + requseterDomainSpecificRequestType + "(" + frontLetterToLower(apiCall.ParentApi.ApiName) + ", " + requesterDomainSpecificCallbackArgsWithoutTypes + commaMaybe + apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false, true) + ");\n    invokeRequest(request);\n}\n");
    return ret;
}
QString RpcGenerator::apiCallToConstructorInitializationCpp(ApiCall apiCall)
{
    QString ret;
    Q_FOREACH(ApiCallArg currentApiCallArg, apiCall.RequestArgs)
    {
        QString currentApiCallArgName = currentApiCallArg.ApiCallArgName;
        ret.append("\n        , m_" + frontLetterToUpper(currentApiCallArgName) + "(" + currentApiCallArgName + ")");
    }
    return ret;
}
QString RpcGenerator::apiCallRequestArgsToRequestInterfaceHeaderDefinitions(ApiCall apiCall)
{
    QString ret;
    if(!apiCall.RequestArgs.isEmpty())
        ret.append("\nprivate:");
    Q_FOREACH(ApiCallArg currentApiCallArg, apiCall.RequestArgs)
    {
        ret.append("\n    " + currentApiCallArg.ApiCallArgType + "m_" + frontLetterToUpper(currentApiCallArg.ApiCallArgName) + ";");
    }
    return ret;
}
QString RpcGenerator::apiCallRequestInterfaceArgMemberNamesWithLeadingCommaspaceIfAndArgs(ApiCall apiCall)
{
    QString ret;
    Q_FOREACH(ApiCallArg currentApiCallArg, apiCall.RequestArgs)
    {
        ret.append(", m_" + frontLetterToUpper(currentApiCallArg.ApiCallArgName));
    }
    return ret;
}
GeneratedFile RpcGenerator::generateApiHeaderFile(Api api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    //API Calls (request slots)
    QStringList allApiCallForwardDeclarations;
    QStringList allApiCallsDeclarationsRawCpp;
    Q_FOREACH(ApiCall apiCall, api.ApiCalls)
    {
        allApiCallForwardDeclarations.append(apiCallToForwardDefinitionRawCpp(&apiCall));
        allApiCallsDeclarationsRawCpp.append(apiCallToRawCppDeclaration(&apiCall, true));
    }
    beforeAndAfterStrings.insert("%API_CALLS_FORWARD_DECLARATIONS%", allApiCallForwardDeclarations.join("\n"));
    beforeAndAfterStrings.insert("%API_CALLS_DECLARATIONS%", allApiCallsDeclarationsRawCpp.join("\n"));
    //API Responses (response pseudo-"signals", which are both Wt and Qt compatible)
    QStringList allApiCallResponseDeclarationsRawCpp;
    Q_FOREACH(ApiCall apiCall, api.ApiCalls)
    {
        allApiCallResponseDeclarationsRawCpp.append(apiCallToRawCppDeclaration(&apiCall, false));
    }
    beforeAndAfterStrings.insert("%API_CALL_RESPONSES_DECLARATIONS%", allApiCallResponseDeclarationsRawCpp.join("\n"));


    GeneratedFile generatedFile(fileToString(":/cleanroom.h"), outputDir.path() + QDir::separator() + classHeaderFileName(api.ApiName), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiSourceFile(Api api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    //API Calls (request slots)
    QStringList allApiCallsHeaderIncludes;
    QStringList allApiCallsDefinitions;
    QStringList allApiCallResponsesDefinitions;
    Q_FOREACH(ApiCall apiCall, api.ApiCalls)
    {
        allApiCallsHeaderIncludes.append(apiCallToRequestInterfaceHeaderInclude(&apiCall));
        allApiCallsDefinitions.append(apiCallToApiDefinitionRawCpp(&apiCall, true));
        allApiCallResponsesDefinitions.append(apiCallToApiDefinitionRawCpp(&apiCall, false));
    }
    beforeAndAfterStrings.insert("%API_CALLS_HEADER_INCLUDES%", allApiCallsHeaderIncludes.join("\n"));
    beforeAndAfterStrings.insert("%API_CALLS_DEFINITIONS%", allApiCallsDefinitions.join("\n"));
    beforeAndAfterStrings.insert("%API_CALLS_RESPONSE_DEFINITIONS%", allApiCallResponsesDefinitions.join("\n"));

    GeneratedFile generatedFile(fileToString(":/cleanroom.cpp"), outputDir.path() + QDir::separator() + classSourceFileName(api.ApiName), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;

}
GeneratedFile RpcGenerator::generateApiRequestInterface(Api api, QDir outputDir)
{
    //this request interface doesn't really do anything at all (since I'm a code gen), but I like it because it's sexy. It could even just be IRequest instead of IBusinessRequest (tons of design opportunities using an IRequest), but whatever. Leaving as IBusinessRequest because I don't have any compelling reason to change it [yet] (KISS)

    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    GeneratedFile generatedFile(fileToString(":/icleanroomrequest.h"), outputDir.path() + QDir::separator() + "i" + api.ApiName.toLower() + "request.h", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestInterfaceHeader(ApiCall apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall.ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    GeneratedFile generatedFile(fileToString(":/icleanroomfrontpagedefaultviewrequest.h"), outputDir.path() + QDir::separator() + "i" + apiCallToRequestBaseName(apiCall).toLower() + ".h", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestInterfaceSource(ApiCall apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall.ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    GeneratedFile generatedFile(fileToString(":/icleanroomfrontpagedefaultviewrequest.cpp"), outputDir.path() + QDir::separator() + "i" + apiCallToRequestBaseName(apiCall).toLower() + ".cpp", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestFromQtHeader(ApiCall apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall.ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);


    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromqt.h"), outputDir.path() + QDir::separator() + classHeaderFileName(apiCallToRequestBaseName(apiCall) + "fromqt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestFromQtSource(ApiCall apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall.ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromqt.cpp"), outputDir.path() + QDir::separator() + classSourceFileName(apiCallToRequestBaseName(apiCall) + "fromqt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestFromWtHeader(ApiCall apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall.ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromwt.h"), outputDir.path() + QDir::separator() + classHeaderFileName(apiCallToRequestBaseName(apiCall) + "fromwt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestFromWtSource(ApiCall apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall.ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromwt.cpp"), outputDir.path() + QDir::separator() + classSourceFileName(apiCallToRequestBaseName(apiCall) + "fromwt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiSessionHeaderFile(Api api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    QString apiCallMethodsOnSessionObjectHeader;
    Q_FOREACH(ApiCall apiCall, api.ApiCalls)
    {
        //FromQt overload
        apiCallMethodsOnSessionObjectHeader.append("    " + apiCallToApiCallRequestMethodDeclarationInSessionHeader(&apiCall, "QObject *objectToCallbackTo, const char *callbackSlot") + ";\n");

        //FromWt overload
        apiCallMethodsOnSessionObjectHeader.append("    " + apiCallToApiCallRequestMethodDeclarationInSessionHeader(&apiCall, "const std::string &wtSessionId, boost::function<void (" + apiCallArgNamesToCommaSeparatedList(&apiCall, false, false, false, true) + ")> wApplicationCallback") + ";\n");
    }
    beforeAndAfterStrings.insert("%API_CALL_METHODS_ON_SESSION_OBJECT_HEADER%", apiCallMethodsOnSessionObjectHeader);

    GeneratedFile generatedFile(fileToString(":/cleanroomsession.h"), outputDir.path() + QDir::separator() + api.ApiName.toLower() + "session.h", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiSessionSourceFile(Api api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    QString apiCallRequestHeaderIncludes;
    QString apiCallMethodsOnSessionObjectSource;
    Q_FOREACH(ApiCall apiCall, api.ApiCalls)
    {
        //Request includes
        apiCallRequestHeaderIncludes.append("\n#include \"" + apiCallToRequestBaseName(&apiCall).toLower() + "fromqt.h\"");
        apiCallRequestHeaderIncludes.append("\n#include \"" + apiCallToRequestBaseName(&apiCall).toLower() + "fromwt.h\"");

        //FromQt overload
        apiCallMethodsOnSessionObjectSource.append(apiCallToApiCallRequestMethodDefinitionInSessionSource(&apiCall, true));

        //FromWt overload
        apiCallMethodsOnSessionObjectSource.append(apiCallToApiCallRequestMethodDefinitionInSessionSource(&apiCall, false));
    }
    beforeAndAfterStrings.insert("%API_CALL_REQUEST_HEADER_INCLUDES%", apiCallRequestHeaderIncludes);
    beforeAndAfterStrings.insert("%API_CALL_METHODS_ON_SESSION_OBJECT_SOURCE%", apiCallMethodsOnSessionObjectSource);

    GeneratedFile generatedFile(fileToString(":/cleanroomsession.cpp"), outputDir.path() + QDir::separator() + api.ApiName.toLower() + "session.cpp", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateNewSessionRequestInterface(Api api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    GeneratedFile generatedFile(fileToString(":/icleanroomnewsessionrequest.h"), outputDir.path() + QDir::separator() + "i" + api.ApiName.toLower() + "newsessionrequest.h", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateNewSessionRequestFromHeaderFile(Api api, QDir outputDir, const QString &qtOrWtFileNameSuffix)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    GeneratedFile generatedFile(fileToString(":/cleanroomnewsessionrequestfrom" + qtOrWtFileNameSuffix + ".h"), outputDir.path() + QDir::separator() + api.ApiName.toLower() + "newsessionrequestfrom" + qtOrWtFileNameSuffix + ".h", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateNewSessionRequestFromSourceFile(Api api, QDir outputDir, const QString &qtOrWtFileNameSuffix)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    GeneratedFile generatedFile(fileToString(":/cleanroomnewsessionrequestfrom" + qtOrWtFileNameSuffix + ".cpp"), outputDir.path() + QDir::separator() + api.ApiName.toLower() + "newsessionrequestfrom" + qtOrWtFileNameSuffix + ".cpp", beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
TemplateBeforeAndAfterStrings_Type RpcGenerator::initialBeforeAndAfterStrings(Api api)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings;

    beforeAndAfterStrings.insert("%API_NAME%", api.ApiName);
    beforeAndAfterStrings.insert("%API_NAME_LOWERCASE%", api.ApiName.toLower());
    beforeAndAfterStrings.insert("%API_NAME_UPPERCASE%", api.ApiName.toUpper());
    beforeAndAfterStrings.insert("%API_AS_VARIABLE_NAME%", frontLetterToLower(api.ApiName));

    return beforeAndAfterStrings;
}
void RpcGenerator::appendApiCallBeforeAndAfterStrings(TemplateBeforeAndAfterStrings_Type *beforeAndAfterStrings, ApiCall apiCall)
{
    beforeAndAfterStrings->insert("%API_CALL_TO_REQUEST_BASE_NAME_FRONT_CHAR_TO_LOWER%", frontLetterToLower(apiCallToRequestBaseName(apiCall)));
    beforeAndAfterStrings->insert("%API_CALL_REQUEST_INTERFACE_HEADER_INCLUDE%", apiCallToRequestInterfaceHeaderInclude(apiCall));
    beforeAndAfterStrings->insert("%API_CALL_TO_REQUEST_BASE_NAME%", apiCallToRequestBaseName(apiCall));
    beforeAndAfterStrings->insert("%API_CALL_SLOT_NAME%", apiCall.ApiCallSlotName);
    beforeAndAfterStrings->insert("%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%", apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false));
    beforeAndAfterStrings->insert("%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_TYPENAMES%", apiCallArgNamesToCommaSeparatedList(apiCall, false, false, true));
    beforeAndAfterStrings->insert("%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_VARNAMES%", apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false, true));
    beforeAndAfterStrings->insert("%API_CALL_SLOT_NAME_TO_UPPER%", apiCall.ApiCallSlotName.toUpper());
    beforeAndAfterStrings->insert("%API_CALL_REQUEST_FROM_QT_HEADER_FILENAME%", classHeaderFileName(apiCallToRequestBaseName(apiCall) + "fromqt"));
    beforeAndAfterStrings->insert("%API_CALL_TO_REQUEST_BASE_NAME_TO_LOWERCASE%", apiCallToRequestBaseName(apiCall).toLower());
    beforeAndAfterStrings->insert("%API_CALL_ARG_TYPES_AND_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%", (apiCall.RequestArgs.isEmpty() ? "" : ", ") + apiCallArgNamesToCommaSeparatedList(apiCall, true, false));
    beforeAndAfterStrings->insert("%API_CALL_ARG_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%", (apiCall.RequestArgs.isEmpty() ? "" : ", ") + apiCallArgNamesToCommaSeparatedList(apiCall, true, false, true));
    beforeAndAfterStrings->insert("%API_CALL_ARG_CONSTRUCTOR_INITIALIZATION_LIST%", apiCallToConstructorInitializationCpp(apiCall));
    beforeAndAfterStrings->insert("%API_CALL_ARG_MEMBER_DEFINITIONS_IN_REQUEST_INTERFACE_HEADER%", apiCallRequestArgsToRequestInterfaceHeaderDefinitions(apiCall));
    beforeAndAfterStrings->insert("%API_CALL_REQUEST_INTERFACE_ARG_MEMBER_NAMES_WITH_LEADING_COMMASPACE_IF_ANY_ARGS%", apiCallRequestInterfaceArgMemberNamesWithLeadingCommaspaceIfAndArgs(apiCall));
}
void RpcGenerator::writeApiCallFiles(FilesToWriteType *filesToWrite, ApiCall apiCall, QDir outputDir)
{
    //Interface
    filesToWrite->insert(generateApiCallRequestInterfaceHeader(apiCall, outputDir));
    filesToWrite->insert(generateApiCallRequestInterfaceSource(apiCall, outputDir));

    //Qt Impl
    filesToWrite->insert(generateApiCallRequestFromQtHeader(apiCall, outputDir));
    filesToWrite->insert(generateApiCallRequestFromQtSource(apiCall, outputDir));

    //Wt Impl
    filesToWrite->insert(generateApiCallRequestFromWtHeader(apiCall, outputDir));
    filesToWrite->insert(generateApiCallRequestFromWtSource(apiCall, outputDir));
}
bool RpcGenerator::generateRpcActual(Api api, QString outputPath)
{
    QDir outputDir(outputPath);
    FilesToWriteType filesToWrite;

    //Main API "Business"
    filesToWrite.insert(generateApiHeaderFile(api, outputDir));
    filesToWrite.insert(generateApiSourceFile(api, outputDir));

    filesToWrite.insert(generateApiRequestInterface(api, outputDir));

    //Per-API-Call Objects
    Q_FOREACH(ApiCall apiCall, api.ApiCalls)
    {
        writeApiCallFiles(&filesToWrite, &apiCall, outputDir);
    }

    //Session
    filesToWrite.insert(generateApiSessionHeaderFile(api, outputDir));
    filesToWrite.insert(generateApiSessionSourceFile(api, outputDir));
    //New Session Request Interface
    filesToWrite.insert(generateNewSessionRequestInterface(api, outputDir));
    //New Session Request //from Qt
    filesToWrite.insert(generateNewSessionRequestFromHeaderFile(api, outputDir, "qt"));
    filesToWrite.insert(generateNewSessionRequestFromSourceFile(api, outputDir, "qt"));
    //New Session Request //from Wt
    filesToWrite.insert(generateNewSessionRequestFromHeaderFile(api, outputDir, "wt"));
    filesToWrite.insert(generateNewSessionRequestFromSourceFile(api, outputDir, "wt"));

    if(!writeFiles(filesToWrite))
        EEEEEEEEEE_RETURN_RpcGenerator("failed to write some files", false)
    return true;
}

QString RpcGenerator::fileToString(QString filePath)
{
    QString ret;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        ret.append(stream.readAll());
    }
    else
    {
        emit e("failed to open: " + filePath);
    }
    return ret;
}
bool RpcGenerator::writeFiles(FilesToWriteType filesToWrite)
{
    FilesToWriteIterator it(filesToWrite);
    while(it.hasNext())
    {
        const FileToWriteType currentFileToWrite = it.next();
        if(!writeFile(currentFileToWrite.GeneratedFilePath, currentFileToWrite.GeneratedFileContents))
            EEEEEEEEEE_RETURN_RpcGenerator("failed to write file: " + currentFileToWrite.GeneratedFilePath, false)
    }
    return true;
}
bool RpcGenerator::writeFile(QString filePath, QString fileContents)
{
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        EEEEEEEEEE_RETURN_RpcGenerator("failed to open file for writing: " + filePath, false)
    QTextStream stream(&file);
    stream << fileContents;
    return true;
}
void GeneratedFile::replaceTemplateBeforesWithAfters()
{
    TemplateBeforeAndAfterStringsIterator it(TemplateBeforeAndAfterStrings);
    while(it.hasNext())
    {
        it.next();
        GeneratedFileContents.replace(it.key(), it.value());
    }
}
