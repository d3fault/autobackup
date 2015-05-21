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
QString RpcGenerator::apiHeaderFileName(QString apiName)
{
    return apiName.toLower() + ".h";
}
QString RpcGenerator::classNameToHeaderIncludeGuard(QString apiName)
{
    return apiName.toUpper() + "_H";
}
QString RpcGenerator::apiCallToCpp(ApiCall *apiCall, bool requestIfTrue)
{
    QString ret("void " + apiCall->ApiCallSlotName + QString(requestIfTrue ? "" : "Finished"));
    QStringList apiCallArgsRawCpp;
    Q_FOREACH(ApiCallArg apiCallArg, (requestIfTrue ? apiCall->RequestArgs : apiCall->ResponseArgs))
    {
        apiCallArgsRawCpp.append(apiCallArgToCpp(&apiCallArg));
    }
    ret.append("(" + apiCallArgsRawCpp.join(", ") + ");");
    return ret;
}
QString RpcGenerator::apiCallArgToCpp(ApiCallArg *apiCallArg)
{
    return apiCallArg->ApiCallArgType + " " + apiCallArg->ApiCallArgName;
}
GeneratedFile RpcGenerator::generateApiHeaderFile(Api *api, QDir outputDir)
{
    TemplateBeforeAndAfterStrings_Type beforeAndAfterStrings;

    beforeAndAfterStrings.insert("%API_NAME%", api->ApiName);
    beforeAndAfterStrings.insert("%API_NAME_INCLUDE_GUARD%", classNameToHeaderIncludeGuard(api->ApiName));

    //API Calls (request slots)
    QStringList allApiCallsDeclarationsRawCpp;
    Q_FOREACH(ApiCall apiCall, api->ApiCalls)
    {
        allApiCallsDeclarationsRawCpp.append(apiCallToCpp(&apiCall, true));
    }
    beforeAndAfterStrings.insert("%API_CALLS_DECLARATIONS%", allApiCallsDeclarationsRawCpp.join("\n"));
    //API Responses (response pseudo-"signals", which are both Wt and Qt compatible)
    QStringList allApiCallResponseDeclarationsRawCpp;
    Q_FOREACH(ApiCall apiCall, api->ApiCalls)
    {
        allApiCallResponseDeclarationsRawCpp.append(apiCallToCpp(&apiCall, false));
    }


    GeneratedFile generatedFile(fileToString(":/apiHeaderFileTemplate.t"), outputDir.path() + QDir::separator() + apiHeaderFileName(api->ApiName), beforeAndAfterStrings);
    generatedFile.replaceTemplateBeforesWithAfters();
    return generatedFile;
}
GeneratedFile RpcGenerator::generateApiSourceFile(Api *api, QDir outputDir)
{
    //TODOreq
    //return GeneratedFile()
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
