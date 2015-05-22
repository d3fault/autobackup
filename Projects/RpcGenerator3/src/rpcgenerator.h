#ifndef RPCGENERATOR_H
#define RPCGENERATOR_H

#include <QObject>
#include <QSet>
#include <QHash>
#include <QSetIterator>
#include <QHashIterator>
#include <QDir>

class Api;
class ApiCall;
class ApiCallArg;

#define TemplateBeforeAndAfter_Type QString /*before*/, QString /*after */
typedef QHash<TemplateBeforeAndAfter_Type> TemplateBeforeAndAfterStrings_Type;
typedef QHashIterator<TemplateBeforeAndAfter_Type> TemplateBeforeAndAfterStringsIterator;

struct GeneratedFile
{
    GeneratedFile(QString templateFileContents, QString generatedFileName, TemplateBeforeAndAfterStrings_Type templateBeforeAndAfterStrings = TemplateBeforeAndAfterStrings_Type())
        : TemplateFileContents(templateFileContents)
        , GeneratedFilePath(generatedFileName)
        , GeneratedFileContents(templateFileContents)
        , TemplateBeforeAndAfterStrings(templateBeforeAndAfterStrings)
    { }
    QString TemplateFileContents;
    QString GeneratedFilePath;
    QString GeneratedFileContents;
    TemplateBeforeAndAfterStrings_Type TemplateBeforeAndAfterStrings;

    void replaceTemplateBeforesWithAfters();
};
inline bool operator==(const GeneratedFile &g1, const GeneratedFile &g2)
{
    return (g1.GeneratedFilePath == g2.GeneratedFilePath);
}
inline uint qHash(const GeneratedFile &key, uint seed)
{
    return qHash(key.GeneratedFilePath, seed);
}

typedef GeneratedFile FileToWriteType;
typedef QSet<FileToWriteType> FilesToWriteType;
typedef QSetIterator<FileToWriteType> FilesToWriteIterator;

class RpcGenerator : public QObject
{
    Q_OBJECT
public:
    explicit RpcGenerator(QObject *parent = 0);
private:
    static QString frontLetterToUpper(const QString &stringInput);
    static QString apiHeaderFileName(QString apiName);
    static QString apiSourceFileName(QString apiName);
    static QString apiCallToRequestInterfaceTypeName(ApiCall *apiCall);
    static QString apiCallToRequestInterfaceHeaderInclude(ApiCall *apiCall);
    static QString apiCallToForwardDefinitionRawCpp(ApiCall *apiCall);
    static QString apiCallArgNamesToCommaSeparatedList(ApiCall *apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter = true, bool emitTypes = true);
    static QString apiCallToMethodCppSignature(ApiCall *apiCall, bool requestIfTrue, bool makeRequestPointerFirstParameter = true);
    static QString apiCallToRawCppDeclaration(ApiCall *apiCall, bool requestIfTrue);
    static QString apiCallArgToCpp(ApiCallArg *apiCallArg, bool emitTypes = true);
    static QString apiCallToApiDefinitionRawCpp(ApiCall *apiCall, bool requestIfTrue);

    GeneratedFile generateApiHeaderFile(Api* api, QDir outputDir);
    GeneratedFile generateApiSourceFile(Api* api, QDir outputDir);

    TemplateBeforeAndAfterStrings_Type initialBeforeAndAfterStrings(Api *api);

    bool generateRpcActual(Api *api, QString outputPath);
    QString fileToString(QString filePath);
    bool writeFiles(FilesToWriteType filesToWrite);
    bool writeFile(QString filePath, QString fileContents);
signals:
    void o(QString);
    void e(QString);
    void rpcGenerated(bool success = false, const QString &directoryFilesWereGeneratedInto = QString());
public slots:
    void generateRpc();
};

#endif // RPCGENERATOR_H
