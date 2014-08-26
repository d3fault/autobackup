#include "designequalsimplementationlenientsignalorslotsignaturerparser.h"

#include "libclangpch.h"
using namespace clang;

class DesignEqualsImplementationLenientSignalOrSlotSignaturerParserRecursiveAstVisitor : public RecursiveASTVisitor<DesignEqualsImplementationLenientSignalOrSlotSignaturerParserRecursiveAstVisitor>
{
public:
    explicit DesignEqualsImplementationLenientSignalOrSlotSignaturerParserRecursiveAstVisitor(ASTContext *context, DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *designEqualsImplementationFunctionDefinitionParser)
        : m_Context(context)
        , m_DesignEqualsImplementationFunctionDefinitionParser(designEqualsImplementationFunctionDefinitionParser)
    { }
    bool VisitFunctionDecl(FunctionDecl *functionDecl)
    {
        ++m_DesignEqualsImplementationFunctionDefinitionParser->m_NumEncounteredFunctionDeclarations;
        m_DesignEqualsImplementationFunctionDefinitionParser->m_ParsedFunctionName = QString::fromStdString(functionDecl->getNameAsString());
        //TO DOnereq(can't instantiate internal clang types eaisly): should i config a DesignEqualsImplementationMethod (rename to function?) or should i pass around the clang internal types? I'm leaning towards "depend on clang as little as possible (especially if i want to have a compile time switch to not even use it)", even though admittedly I'll be reinventing the wheel here and there (like I already am/have :-P). I'm still iffy on how I'm going to handle a variable "how the user typed it" (const SomeClass &blah -- etc) vs "underlying type" (SomeClass)... but I'm damn glad clang has allowed me to DETERMINE the underlying class :)
        unsigned paramCount = functionDecl->getNumParams();
        for(unsigned i = 0; i < paramCount; ++i)
        {
            const ParmVarDecl *currentParam = functionDecl->getParamDecl(i);
            QualType currentParamQualType = currentParam->getType(); //tempted to hang onto the QualType itself, but idfk how to deserialize/load back into one later on :(
            m_DesignEqualsImplementationFunctionDefinitionParser->m_ParsedFunctionArguments.append(qMakePair(QString::fromStdString(currentParamQualType.getAsString()), QString::fromStdString(currentParam->getNameAsString())));
        }
        return true;
    }
private:
    ASTContext *m_Context;
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *m_DesignEqualsImplementationFunctionDefinitionParser;
};

class DesignEqualsImplementationLenientSignalOrSlotSignaturerParserAstConsumer : public clang::ASTConsumer
{
public:
    explicit DesignEqualsImplementationLenientSignalOrSlotSignaturerParserAstConsumer(ASTContext *context, DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *designEqualsImplementationFunctionDefinitionParser)
        : m_Visitor(context, designEqualsImplementationFunctionDefinitionParser)
    { }
    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        m_Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParserRecursiveAstVisitor m_Visitor;
};

struct DesignEqualsImplementationLenientSignalOrSlotSignaturerParserDiagnosticConsumer : clang::DiagnosticConsumer
{
    llvm::OwningPtr<DiagnosticConsumer> m_Proxy;
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParserDiagnosticConsumer(DiagnosticConsumer *previous, DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *designEqualsImplementationFunctionDefinitionParser)
        : m_Proxy(previous)
        , m_DesignEqualsImplementationFunctionDefinitionParser(designEqualsImplementationFunctionDefinitionParser)
    { }
    void BeginSourceFile(const clang::LangOptions& langOpts, const clang::Preprocessor* preProcessor = 0) { m_Proxy->BeginSourceFile(langOpts, preProcessor); }
    void clear(){ m_Proxy->clear(); }
    void EndSourceFile() { m_Proxy->EndSourceFile(); }
    void finish() { m_Proxy->finish(); }
    void HandleDiagnostic(clang::DiagnosticsEngine::Level diagLevel, const clang::Diagnostic &diagnosticInfo)
    {
        if(diagLevel == DiagnosticsEngine::Error && (diagnosticInfo.getID() == clang::diag::err_unknown_typename || diagnosticInfo.getID() == clang::diag::err_unknown_typename_suggest) && diagnosticInfo.getNumArgs() > 0)
        {
            m_DesignEqualsImplementationFunctionDefinitionParser->m_UnknownTypesDetectedInLastRunToolOnCodeIteration.append(QString::fromStdString(diagnosticInfo.getArgIdentifier(0)->getName().str()));
        }
        DiagnosticConsumer::HandleDiagnostic(diagLevel, diagnosticInfo);
        m_Proxy->HandleDiagnostic(diagLevel, diagnosticInfo);
    }
private:
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *m_DesignEqualsImplementationFunctionDefinitionParser;
};

class DesignEqualsImplementationLenientSignalOrSlotSignaturerParserAstFrontendAction : public clang::ASTFrontendAction
{
public:
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParserAstFrontendAction(DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *designEqualsImplementationFunctionDefinitionParser)
        : m_DesignEqualsImplementationFunctionDefinitionParser(designEqualsImplementationFunctionDefinitionParser)
    { }
    virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef InFile)
    {
        Q_UNUSED(InFile)
        compiler.getDiagnostics().setClient(new DesignEqualsImplementationLenientSignalOrSlotSignaturerParserDiagnosticConsumer(compiler.getDiagnostics().takeClient(), m_DesignEqualsImplementationFunctionDefinitionParser));
        return new DesignEqualsImplementationLenientSignalOrSlotSignaturerParserAstConsumer(&compiler.getASTContext(), m_DesignEqualsImplementationFunctionDefinitionParser);
    }
private:
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParser *m_DesignEqualsImplementationFunctionDefinitionParser;
};

//lenient because it allows the "void " return type to be missing (but if return type is specified, it must be void), allows the parenthesis to be missing, and allows the semi-colon to be missing. in the simplest case, typing the signal/slot name alone is sufficient (i can't remember why i even parse in that case, but fuck it :-P... one code path is one code path)
DesignEqualsImplementationLenientSignalOrSlotSignaturerParser::DesignEqualsImplementationLenientSignalOrSlotSignaturerParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend)
    : m_NumEncounteredFunctionDeclarations(0)
    , m_HasError(false)
{
    QString mutableFunctionDeclaration(functionDeclaration);
    if(!mutableFunctionDeclaration.trimmed().startsWith("void "))
    {
        //TODOreq: handle if they put a non-void return type somehow, idfk how though lol. we'll prepend void and then it's syntax error? basically i don't even want them to type the return type since it must be void...
        mutableFunctionDeclaration.prepend("void ");
    }
    if(!mutableFunctionDeclaration.contains("("))
    {
        //no args specified (they typed name ONLY), we need to append a "()" for them in order to not get a syntax error
        if(mutableFunctionDeclaration.contains(";") || mutableFunctionDeclaration.contains(")"))
        {
            //shit we can't recover from that, syntax error
            m_MostRecentError = "you have a ')' or a ';', but no '(' -- so a special syntax error";
            m_HasError = true;
            return;
        }
        mutableFunctionDeclaration.append("()");
    }
    if(!mutableFunctionDeclaration.trimmed().endsWith(";"))
    {
        mutableFunctionDeclaration.append(";");
    }
    Q_FOREACH(const QString &currentKnownType, knownTypesToTypedefPrepend)
    {
        //TODOoptional: i could wrap these known types with a magical string (begin, end) and in a comment so that they aren't displayed to the user syntax error
        mutableFunctionDeclaration.prepend("typedef int " + currentKnownType + ";\n"); //NVM: using append instead of prepend as originally planned because the ordering will matter later on i think, oh shit no it won't. thought it would because of interfaces/inheritence... but nope i just need to not get unknown type errors is all :)
    }
    std::string functionDeclarationStdString = mutableFunctionDeclaration.toStdString();
    while(!clang::tooling::runToolOnCode(new DesignEqualsImplementationLenientSignalOrSlotSignaturerParserAstFrontendAction(this), functionDeclarationStdString))
    {
        if(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.isEmpty())
        {
            m_MostRecentError = "libclang reported a syntax error. Check stderr for the details";
            m_HasError = true;
            return;
        }
        mutableFunctionDeclaration.prepend("typedef int " + m_UnknownTypesDetectedInLastRunToolOnCodeIteration.at(0) + ";\n"); //just one at a time, since the next one might be the same unknown type (right?)
        m_NewTypesSeenInFunctionDeclaration.append(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.at(0));
        m_UnknownTypesDetectedInLastRunToolOnCodeIteration.clear();
        m_ParsedFunctionArguments.clear();
        m_NumEncounteredFunctionDeclarations = 0;
        functionDeclarationStdString = mutableFunctionDeclaration.toStdString();
    }

    //runToolOnCode success = no unrecoverable syntax errors

    if(m_NumEncounteredFunctionDeclarations != 1)
    {
        m_MostRecentError = "error: " + QString::number(m_NumEncounteredFunctionDeclarations) + " function signatures parsed, but expected exactly 1";
        m_HasError = true;
        return;
    }
}
bool DesignEqualsImplementationLenientSignalOrSlotSignaturerParser::hasError() const
{
    return m_HasError;
}
//only valid if has error
QString DesignEqualsImplementationLenientSignalOrSlotSignaturerParser::mostRecentError() const
{
    return m_MostRecentError;
}
QString DesignEqualsImplementationLenientSignalOrSlotSignaturerParser::parsedFunctionName() const
{
    //TODOreq: error out if zero or two+ function declarations were seen
    return m_ParsedFunctionName;
}
QList<QString> DesignEqualsImplementationLenientSignalOrSlotSignaturerParser::newTypesSeenInFunctionDeclaration() const
{
    return m_NewTypesSeenInFunctionDeclaration;
}
QList<MethodArgumentTypedef> DesignEqualsImplementationLenientSignalOrSlotSignaturerParser::parsedFunctionArguments() const
{
    return m_ParsedFunctionArguments;
}
