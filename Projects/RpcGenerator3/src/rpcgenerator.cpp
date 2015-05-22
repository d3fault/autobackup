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
QString RpcGenerator::apiHeaderFileName(QString apiName)
{
    return apiName.toLower() + ".h";
}
QString RpcGenerator::apiSourceFileName(QString apiName)
{
    return apiName.toLower() + ".cpp";
}
QString RpcGenerator::apiCallToRequestInterfaceTypeName(ApiCall *apiCall)
{
    return "I" + apiCall->ParentApi->ApiName + frontLetterToUpper(apiCall->ApiCallSlotName) + "Request";
}
QString RpcGenerator::apiCallToRequestInterfaceHeaderInclude(ApiCall *apiCall)
{
    return "#include \"" + apiCallToRequestInterfaceTypeName(apiCall).toLower() + ".h\"";
}
QString RpcGenerator::apiCallToForwardDefinitionRawCpp(ApiCall *apiCall)
{
    return "class " + apiCallToRequestInterfaceTypeName(apiCall) + ";";
}
QString RpcGenerator::apiCallArgNamesToCommaSeparatedList(ApiCall *apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter, bool emitTypes)
{
    QStringList apiCallArgsRawCpp;
    if(makeRequestPointerFirstParameter)
        apiCallArgsRawCpp.append(apiCallToRequestInterfaceTypeName(apiCall) + " *request");
    Q_FOREACH(ApiCallArg apiCallArg, (requestIfTrue ? apiCall->RequestArgs : apiCall->ResponseArgs))
    {
        apiCallArgsRawCpp.append(apiCallArgToCpp(&apiCallArg, emitTypes));
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
QString RpcGenerator::apiCallArgToCpp(ApiCallArg *apiCallArg, bool emitTypes)
{
    if(emitTypes)
        return apiCallArg->ApiCallArgType + " " + apiCallArg->ApiCallArgName;
    return apiCallArg->ApiCallArgName;
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
        ret.append("request->respond(" + apiCallArgNamesToCommaSeparatedList(apiCall, false, false, false) + ");");
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


    GeneratedFile generatedFile(fileToString(":/cleanroom.h"), outputDir.path() + QDir::separator() + apiHeaderFileName(api->ApiName), beforeAndAfterStrings);
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

    GeneratedFile generatedFile(fileToString(":/cleanroom.cpp"), outputDir.path() + QDir::separator() + apiSourceFileName(api->ApiName), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;

}
TemplateBeforeAndAfterStrings_Type RpcGenerator::initialBeforeAndAfterStrings(Api *api)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings;

    beforeAndAfterStrings.insert("%API_NAME%", api->ApiName);
    beforeAndAfterStrings.insert("%API_NAME_LOWERCASE%", api->ApiName.toLower());
    beforeAndAfterStrings.insert("%API_NAME_UPPERCASE%", api->ApiName.toUpper());

    return beforeAndAfterStrings;
}
bool RpcGenerator::generateRpcActual(Api *api, QString outputPath)
{
    QDir outputDir(outputPath);
    FilesToWriteType filesToWrite;

    filesToWrite.insert(generateApiHeaderFile(api, outputDir));
    filesToWrite.insert(generateApiSourceFile(api, outputDir));

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

    Api api("CleanRoom");
    api.createApiCall("getFrontPageDefaultView", QList<ApiCallArg>(), QList<ApiCallArg>() << ApiCallArg("QStringList", "frontPageDocs"));
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
