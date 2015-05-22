#include "rpcgenerator.h"

#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "api.h"

#define EEEEEEEEEE_RETURN_RpcGenerator(errorMessage, returnWhat) \
{ \
    emit e(errorMessage); \
    emit rpcGenerated(false); \
    return returnWhat; \
}

#define EEEEEEEEEE_RpcGenerator(errorMessage) EEEEEEEEEE_RETURN_RpcGenerator(errorMessage, )

RpcGenerator::RpcGenerator(QObject *parent)
    : QObject(parent)
{ }
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
QString RpcGenerator::apiCallToRequestBaseName(ApiCall *apiCall)
{
    return apiCall->ParentApi->ApiName + frontLetterToUpper(apiCall->ApiCallSlotName) + "Request";
}
QString RpcGenerator::apiCallToRequestInterfaceTypeName(ApiCall *apiCall)
{
    return "I" + apiCallToRequestBaseName(apiCall);
}
QString RpcGenerator::apiCallToRequestInterfaceHeaderInclude(ApiCall *apiCall)
{
    return "#include \"" + apiCallToRequestInterfaceTypeName(apiCall).toLower() + ".h\"";
}
QString RpcGenerator::apiCallToForwardDefinitionRawCpp(ApiCall *apiCall)
{
    return "class " + apiCallToRequestInterfaceTypeName(apiCall) + ";";
}
QString RpcGenerator::apiCallArgNamesToCommaSeparatedList(ApiCall *apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter, bool emitTypes, bool emitNames)
{
    QStringList apiCallArgsRawCpp;
    if(makeRequestPointerFirstParameter)
        apiCallArgsRawCpp.append(apiCallToRequestInterfaceTypeName(apiCall) + " *request");
    Q_FOREACH(ApiCallArg apiCallArg, (requestIfTrue ? apiCall->RequestArgs : apiCall->ResponseArgs))
    {
        apiCallArgsRawCpp.append(apiCallArgToCpp(&apiCallArg, emitTypes, emitNames));
    }
    return apiCallArgsRawCpp.join(", ");
}
QString RpcGenerator::apiCallToMethodCppSignature(ApiCall *apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter)
{
    QString ret(apiCall->ApiCallSlotName + QString(requestIfTrue ? "" : "Finished"));
    QString apiCallArgsAsCommaSeparatedList = apiCallArgNamesToCommaSeparatedList(apiCall, requestIfTrue, makeRequestPointerFirstParameter);
    ret.append("(" + apiCallArgsAsCommaSeparatedList + ")");
    return ret;
}
QString RpcGenerator::apiCallToRawCppDeclaration(ApiCall *apiCall, bool requestIfTrue)
{
    QString ret("    void " + apiCallToMethodCppSignature(apiCall, requestIfTrue) + ";");
    return ret;
}
QString RpcGenerator::apiCallArgToCpp(ApiCallArg *apiCallArg, bool emitTypes, bool emitNames)
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
QString RpcGenerator::apiCallToApiDefinitionRawCpp(ApiCall *apiCall, bool requestIfTrue)
{
    QString methodSignatureForDefinition = apiCall->ParentApi->ApiName + "::" + apiCallToMethodCppSignature(apiCall, requestIfTrue);
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
GeneratedFile RpcGenerator::generateApiHeaderFile(Api *api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    //API Calls (request slots)
    QStringList allApiCallForwardDeclarations;
    QStringList allApiCallsDeclarationsRawCpp;
    Q_FOREACH(ApiCall apiCall, api->ApiCalls)
    {
        allApiCallForwardDeclarations.append(apiCallToForwardDefinitionRawCpp(&apiCall));
        allApiCallsDeclarationsRawCpp.append(apiCallToRawCppDeclaration(&apiCall, true));
    }
    beforeAndAfterStrings.insert("%API_CALLS_FORWARD_DECLARATIONS%", allApiCallForwardDeclarations.join("\n"));
    beforeAndAfterStrings.insert("%API_CALLS_DECLARATIONS%", allApiCallsDeclarationsRawCpp.join("\n"));
    //API Responses (response pseudo-"signals", which are both Wt and Qt compatible)
    QStringList allApiCallResponseDeclarationsRawCpp;
    Q_FOREACH(ApiCall apiCall, api->ApiCalls)
    {
        allApiCallResponseDeclarationsRawCpp.append(apiCallToRawCppDeclaration(&apiCall, false));
    }
    beforeAndAfterStrings.insert("%API_CALL_RESPONSES_DECLARATIONS%", allApiCallResponseDeclarationsRawCpp.join("\n"));


    GeneratedFile generatedFile(fileToString(":/cleanroom.h"), outputDir.path() + QDir::separator() + classHeaderFileName(api->ApiName), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiSourceFile(Api *api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(api);

    //API Calls (request slots)
    QStringList allApiCallsHeaderIncludes;
    QStringList allApiCallsDefinitions;
    QStringList allApiCallResponsesDefinitions;
    Q_FOREACH(ApiCall apiCall, api->ApiCalls)
    {
        allApiCallsHeaderIncludes.append(apiCallToRequestInterfaceHeaderInclude(&apiCall));
        allApiCallsDefinitions.append(apiCallToApiDefinitionRawCpp(&apiCall, true));
        allApiCallResponsesDefinitions.append(apiCallToApiDefinitionRawCpp(&apiCall, false));
    }
    beforeAndAfterStrings.insert("%API_CALLS_HEADER_INCLUDES%", allApiCallsHeaderIncludes.join("\n"));
    beforeAndAfterStrings.insert("%API_CALLS_DEFINITIONS%", allApiCallsDefinitions.join("\n"));
    beforeAndAfterStrings.insert("%API_CALLS_RESPONSE_DEFINITIONS%", allApiCallResponsesDefinitions.join("\n"));

    GeneratedFile generatedFile(fileToString(":/cleanroom.cpp"), outputDir.path() + QDir::separator() + classSourceFileName(api->ApiName), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;

}
GeneratedFile RpcGenerator::generateApiCallRequestFromQtHeader(ApiCall *apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall->ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    beforeAndAfterStrings.insert("%API_CALL_SLOT_NAME_TO_UPPER%", apiCall->ApiCallSlotName.toUpper());

    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromqt.h"), outputDir.path() + QDir::separator() + classHeaderFileName(apiCallToRequestBaseName(apiCall) + "fromqt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestFromQtSource(ApiCall *apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall->ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);

    beforeAndAfterStrings.insert("%API_CALL_REQUEST_FROM_QT_HEADER_FILENAME%", classHeaderFileName(apiCallToRequestBaseName(apiCall) + "fromqt"));

    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromqt.cpp"), outputDir.path() + QDir::separator() + classSourceFileName(apiCallToRequestBaseName(apiCall) + "fromqt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiCallRequestFromWtHeader(ApiCall *apiCall, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings = initialBeforeAndAfterStrings(apiCall->ParentApi);
    appendApiCallBeforeAndAfterStrings(&beforeAndAfterStrings, apiCall);


    GeneratedFile generatedFile(fileToString(":/cleanroomfrontpagedefaultviewrequestfromwt.h"), outputDir.path() + QDir::separator() + classHeaderFileName(apiCallToRequestBaseName(apiCall) + "fromwt"), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
TemplateBeforeAndAfterStrings_Type RpcGenerator::initialBeforeAndAfterStrings(Api *api)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings;

    beforeAndAfterStrings.insert("%API_NAME%", api->ApiName);
    beforeAndAfterStrings.insert("%API_NAME_LOWERCASE%", api->ApiName.toLower());
    beforeAndAfterStrings.insert("%API_NAME_UPPERCASE%", api->ApiName.toUpper());
    beforeAndAfterStrings.insert("%API_AS_VARIABLE_NAME%", frontLetterToLower(api->ApiName));

    return beforeAndAfterStrings;
}
void RpcGenerator::appendApiCallBeforeAndAfterStrings(TemplateBeforeAndAfterStrings_Type *beforeAndAfterStrings, ApiCall *apiCall)
{
    beforeAndAfterStrings->insert("%API_CALL_REQUEST_INTERFACE_HEADER_INCLUDE%", apiCallToRequestInterfaceHeaderInclude(apiCall));
    beforeAndAfterStrings->insert("%API_CALL_TO_REQUEST_BASE_NAME%", apiCallToRequestBaseName(apiCall));
    beforeAndAfterStrings->insert("%API_CALL_SLOT_NAME%", apiCall->ApiCallSlotName);
    beforeAndAfterStrings->insert("%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXCLUDING_REQUEST_POINTER%", apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false));
    beforeAndAfterStrings->insert("%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_TYPENAMES%", apiCallArgNamesToCommaSeparatedList(apiCall, false, false, true));
    beforeAndAfterStrings->insert("%API_CALL_RESPONSE_ARGS_AS_COMMA_SEPARATED_LIST_EXLCUDING_REQUEST_POINTER_AND_VARNAMES%", apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false, true));
}
void RpcGenerator::writeApiCallFiles(FilesToWriteType *filesToWrite, ApiCall *apiCall, QDir outputDir)
{
    //Qt
    filesToWrite->insert(generateApiCallRequestFromQtHeader(apiCall, outputDir));
    filesToWrite->insert(generateApiCallRequestFromQtSource(apiCall, outputDir));

    //Wt
    filesToWrite->insert(generateApiCallRequestFromWtHeader(apiCall, outputDir));
    //filesToWrite->insert(generateApiCallRequestFromWtSource(apiCall, outputDir));
}
bool RpcGenerator::generateRpcActual(Api *api, QString outputPath)
{
    QDir outputDir(outputPath);
    FilesToWriteType filesToWrite;

    //Main API "Business"
    filesToWrite.insert(generateApiHeaderFile(api, outputDir));
    filesToWrite.insert(generateApiSourceFile(api, outputDir));

    //Per-API-Call Objects
    Q_FOREACH(ApiCall apiCall, api->ApiCalls)
    {
        writeApiCallFiles(&filesToWrite, &apiCall, outputDir);
    }

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
void RpcGenerator::generateRpc()
{
    QTemporaryDir tempDir;
    if(!tempDir.isValid())
        EEEEEEEEEE_RpcGenerator("unable to get temp dir")
    QString outputPath = tempDir.path();

    Api api("SomeBusiness");
    api.createApiCall("loginPlx", QList<ApiCallArg>() << ApiCallArg("char *", "loginUser") << ApiCallArg("QByteArray", "passwordHash"), QList<ApiCallArg>() << ApiCallArg("bool", "loginSuccess") << ApiCallArg("QString", "sessionId"));
    if(!generateRpcActual(&api, outputPath))
        EEEEEEEEEE_RpcGenerator("failed to generate")

    tempDir.setAutoRemove(false);
    emit o("successfully generated rpc to: " + outputPath);
    emit rpcGenerated(true, outputPath);
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
