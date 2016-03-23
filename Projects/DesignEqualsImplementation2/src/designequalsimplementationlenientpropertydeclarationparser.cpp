#include "designequalsimplementationlenientpropertydeclarationparser.h"

//this file is copy/paste hackjob from DesignEqualsImplementationLenientSignalOrSlotSignaturerParser (which itself was  copied (merge pending) into FunctionDeclarationParser). i could probably merge the common functionality into a common base etc, but fuck that shit

#include "libclangpch.h"
using namespace clang;

//TODOreq: s/Property/NonFunctionMember
class DesignEqualsImplementationLenientPropertyDeclarationParserRecursiveAstVisitor : public RecursiveASTVisitor<DesignEqualsImplementationLenientPropertyDeclarationParserRecursiveAstVisitor>
{
public:
    explicit DesignEqualsImplementationLenientPropertyDeclarationParserRecursiveAstVisitor(ASTContext *context, DesignEqualsImplementationLenientPropertyDeclarationParser *designEqualsImplementationPropertyDeclarationParser)
        : m_Context(context)
        , m_DesignEqualsImplementationPropertyDeclarationParser(designEqualsImplementationPropertyDeclarationParser)
    { }
    QualType recursivelyDereferenceToUltimatePointeeTypeAndDiscardQualifiersAndReferenceAmpersand(QualType qualifiedType) //TODOreq: put somewhere more... common... TODOoptional: ask on clang website what proper way to do this is. there might be 'qualifiers' I'm forgetting to strip off
    {
        while(qualifiedType->isPointerType())
            qualifiedType = qualifiedType->getPointeeType();
        return qualifiedType.getUnqualifiedType().getNonReferenceType();
    }
    bool VisitVarDecl(VarDecl *varDecl)
    {
        ++m_DesignEqualsImplementationPropertyDeclarationParser->m_NumEncounteredPropertyDeclarations;
        QualType propertyType = varDecl->getType();
        m_DesignEqualsImplementationPropertyDeclarationParser->m_ParsedPropertyQualifiedType = QString::fromStdString(propertyType.getAsString());
        m_DesignEqualsImplementationPropertyDeclarationParser->m_ParsedPropertyName = QString::fromStdString(varDecl->getNameAsString());
        m_DesignEqualsImplementationPropertyDeclarationParser->m_ParsedPropertyUnqualifiedType = QString::fromStdString(recursivelyDereferenceToUltimatePointeeTypeAndDiscardQualifiersAndReferenceAmpersand(propertyType).getAsString());
        if(varDecl->hasInit())
        {
            const Expr *varInit = varDecl->getInit();
            //if(!varInit->isRValue()) //necessary check? idfk
            //    continue;
            SourceRange varInitSourceRange = varInit->getSourceRange();
            if(!varInitSourceRange.isValid())
                return true;
            CharSourceRange varInitCharSourceRange(varInitSourceRange, true);
            if(!varInitCharSourceRange.isValid())
                return true;
            StringRef varInitText = Lexer::getSourceText(varInitCharSourceRange, m_Context->getSourceManager(), m_Context->getLangOpts(), 0);
            m_DesignEqualsImplementationPropertyDeclarationParser->m_HasInit = true;
            m_DesignEqualsImplementationPropertyDeclarationParser->m_OptionalInit = QString::fromStdString(varInitText.str());
        }
        return true;
    }
private:
    ASTContext *m_Context;
    DesignEqualsImplementationLenientPropertyDeclarationParser *m_DesignEqualsImplementationPropertyDeclarationParser;
};

class DesignEqualsImplementationLenientPropertyDeclarationParserAstConsumer : public clang::ASTConsumer
{
public:
    explicit DesignEqualsImplementationLenientPropertyDeclarationParserAstConsumer(ASTContext *context, DesignEqualsImplementationLenientPropertyDeclarationParser *designEqualsImplementationPropertyDeclarationParser)
        : m_Visitor(context, designEqualsImplementationPropertyDeclarationParser)
    { }
    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        m_Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    DesignEqualsImplementationLenientPropertyDeclarationParserRecursiveAstVisitor m_Visitor;
};

struct DesignEqualsImplementationLenientPropertyDeclarationParserDiagnosticConsumer : clang::DiagnosticConsumer
{
    std::unique_ptr<DiagnosticConsumer> m_Proxy;
    DesignEqualsImplementationLenientPropertyDeclarationParserDiagnosticConsumer(DiagnosticConsumer *previous, DesignEqualsImplementationLenientPropertyDeclarationParser *designEqualsImplementationPropertyDeclarationParser)
        : m_Proxy(previous)
        , m_DesignEqualsImplementationPropertyDeclarationParser(designEqualsImplementationPropertyDeclarationParser)
    { }
    void BeginSourceFile(const clang::LangOptions& langOpts, const clang::Preprocessor* preProcessor = 0) { m_Proxy->BeginSourceFile(langOpts, preProcessor); }
    void clear(){ m_Proxy->clear(); }
    void EndSourceFile() { m_Proxy->EndSourceFile(); }
    void finish() { m_Proxy->finish(); }
    void HandleDiagnostic(clang::DiagnosticsEngine::Level diagLevel, const clang::Diagnostic &diagnosticInfo)
    {
        if(diagLevel == DiagnosticsEngine::Error && (diagnosticInfo.getID() == clang::diag::err_unknown_typename || diagnosticInfo.getID() == clang::diag::err_unknown_typename_suggest) && diagnosticInfo.getNumArgs() > 0)
        {
            m_DesignEqualsImplementationPropertyDeclarationParser->m_UnknownTypesDetectedInLastRunToolOnCodeIteration.append(QString::fromStdString(diagnosticInfo.getArgIdentifier(0)->getName().str()));
        }
        DiagnosticConsumer::HandleDiagnostic(diagLevel, diagnosticInfo);
        m_Proxy->HandleDiagnostic(diagLevel, diagnosticInfo);
    }
private:
    DesignEqualsImplementationLenientPropertyDeclarationParser *m_DesignEqualsImplementationPropertyDeclarationParser;
};

class DesignEqualsImplementationLenientPropertyDeclarationParserAstFrontendAction : public clang::ASTFrontendAction
{
public:
    DesignEqualsImplementationLenientPropertyDeclarationParserAstFrontendAction(DesignEqualsImplementationLenientPropertyDeclarationParser *designEqualsImplementationPropertyDeclarationParser)
        : m_DesignEqualsImplementationPropertyDeclarationParser(designEqualsImplementationPropertyDeclarationParser)
    { }
    virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef InFile)
    {
        Q_UNUSED(InFile)
        compiler.getDiagnostics().setClient(new DesignEqualsImplementationLenientPropertyDeclarationParserDiagnosticConsumer(compiler.getDiagnostics().takeClient(), m_DesignEqualsImplementationPropertyDeclarationParser));
        return new DesignEqualsImplementationLenientPropertyDeclarationParserAstConsumer(&compiler.getASTContext(), m_DesignEqualsImplementationPropertyDeclarationParser);
    }
private:
    DesignEqualsImplementationLenientPropertyDeclarationParser *m_DesignEqualsImplementationPropertyDeclarationParser;
};

//lenient because allows semi-colon to be missing
DesignEqualsImplementationLenientPropertyDeclarationParser::DesignEqualsImplementationLenientPropertyDeclarationParser(const QString &propertyDeclaration, const QList<QString> &knownTypesToTypedefPrepend)
    : m_HasInit(false)
    , m_NumEncounteredPropertyDeclarations(0)
    , m_HasError(false)
{
    QString mutablePropertyDeclaration(propertyDeclaration);
    if(!mutablePropertyDeclaration.trimmed().endsWith(";"))
    {
        mutablePropertyDeclaration.append(";");
    }
    Q_FOREACH(const QString &currentKnownType, knownTypesToTypedefPrepend)
    {
        //TODOoptional: i could wrap these known types with a magical string (begin, end) and in a comment so that they aren't displayed to the user syntax error
        mutablePropertyDeclaration.prepend("typedef int " + currentKnownType + ";\n");
    }
    std::string propertyDeclarationStdString = mutablePropertyDeclaration.toStdString();
    while(!clang::tooling::runToolOnCode(new DesignEqualsImplementationLenientPropertyDeclarationParserAstFrontendAction(this), propertyDeclarationStdString))
    {
        if(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.isEmpty())
        {
            m_MostRecentError = "libclang reported a syntax error. Check stderr for the details";
            m_HasError = true;
            return;
        }
        mutablePropertyDeclaration.prepend("typedef int " + m_UnknownTypesDetectedInLastRunToolOnCodeIteration.at(0) + ";\n"); //just one at a time, since the next one might be the same unknown type (right?)
        m_NewTypesSeenInPropertyDeclaration.append(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.at(0));
        m_UnknownTypesDetectedInLastRunToolOnCodeIteration.clear();
        m_HasInit = false;
        m_NumEncounteredPropertyDeclarations = 0;
        propertyDeclarationStdString = mutablePropertyDeclaration.toStdString();
    }

    //runToolOnCode success = no unrecoverable syntax errors

    if(m_NumEncounteredPropertyDeclarations != 1) //TODOoptional: i suppose they could specify any amount > 0, but for now 1 only. hell even signals/slots could do > 0, but fuck it
    {
        m_MostRecentError = "error: " + QString::number(m_NumEncounteredPropertyDeclarations) + " property declarations parsed, but expected exactly 1";
        m_HasError = true;
        return;
    }
}
QString DesignEqualsImplementationLenientPropertyDeclarationParser::parsedPropertyQualifiedType() const
{
    return m_ParsedPropertyQualifiedType;
}
QString DesignEqualsImplementationLenientPropertyDeclarationParser::parsedPropertyUnqualifiedType() const
{
    return m_ParsedPropertyUnqualifiedType;
}
bool DesignEqualsImplementationLenientPropertyDeclarationParser::hasError() const
{
    return m_HasError;
}
//only valid if has error
QString DesignEqualsImplementationLenientPropertyDeclarationParser::mostRecentError() const
{
    return m_MostRecentError;
}
QString DesignEqualsImplementationLenientPropertyDeclarationParser::parsedPropertyName() const
{
    //TODOreq: error out if zero or two+ function declarations were seen
    return m_ParsedPropertyName;
}
bool DesignEqualsImplementationLenientPropertyDeclarationParser::hasInit() const //can't rely on m_OptionalInit being an empty string to determine this, because they might want to initialize to an empty string!
{
    return m_HasInit;
}
QString DesignEqualsImplementationLenientPropertyDeclarationParser::optionalInit() const
{
    return m_OptionalInit;
}
//doesn't need to be a list since a property only has one type, but eh leftover code from signal/slot signature parsing
QList<QString> DesignEqualsImplementationLenientPropertyDeclarationParser::newTypesSeenInPropertyDeclaration() const
{
    return m_NewTypesSeenInPropertyDeclaration;
}
