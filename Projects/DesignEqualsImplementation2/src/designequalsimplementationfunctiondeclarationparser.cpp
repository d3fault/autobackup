#include "designequalsimplementationfunctiondeclarationparser.h"

#include "libclangpch.h"
using namespace clang;

class DesignEqualsImplementationFunctionDeclarationParserRecursiveAstVisitor : public RecursiveASTVisitor<DesignEqualsImplementationFunctionDeclarationParserRecursiveAstVisitor>
{
public:
    explicit DesignEqualsImplementationFunctionDeclarationParserRecursiveAstVisitor(ASTContext *context, DesignEqualsImplementationFunctionDeclarationParser *designEqualsImplementationFunctionDefinitionParser)
        : m_Context(context)
        , m_DesignEqualsImplementationFunctionDefinitionParser(designEqualsImplementationFunctionDefinitionParser)
    { }
    bool VisitFunctionDecl(FunctionDecl *functionDecl)
    {
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
    DesignEqualsImplementationFunctionDeclarationParser *m_DesignEqualsImplementationFunctionDefinitionParser;
};

class DesignEqualsImplementationFunctionDeclarationAstConsumer : public clang::ASTConsumer
{
public:
    explicit DesignEqualsImplementationFunctionDeclarationAstConsumer(ASTContext *context, DesignEqualsImplementationFunctionDeclarationParser *designEqualsImplementationFunctionDefinitionParser)
        : m_Visitor(context, designEqualsImplementationFunctionDefinitionParser)
    { }
    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        m_Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    DesignEqualsImplementationFunctionDeclarationParserRecursiveAstVisitor m_Visitor;
};

struct DesignEqualsImplementationFunctionDeclarationDiagnosticConsumer : clang::DiagnosticConsumer
{
    llvm::OwningPtr<DiagnosticConsumer> m_Proxy;
    DesignEqualsImplementationFunctionDeclarationDiagnosticConsumer(DiagnosticConsumer *previous, DesignEqualsImplementationFunctionDeclarationParser *designEqualsImplementationFunctionDefinitionParser)
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
    DesignEqualsImplementationFunctionDeclarationParser *m_DesignEqualsImplementationFunctionDefinitionParser;
};

class DesignEqualsImplementationFunctionDeclarationAstFrontendAction : public clang::ASTFrontendAction
{
public:
    DesignEqualsImplementationFunctionDeclarationAstFrontendAction(DesignEqualsImplementationFunctionDeclarationParser *designEqualsImplementationFunctionDefinitionParser)
        : m_DesignEqualsImplementationFunctionDefinitionParser(designEqualsImplementationFunctionDefinitionParser)
    { }
    virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef InFile)
    {
        Q_UNUSED(InFile)
        compiler.getDiagnostics().setClient(new DesignEqualsImplementationFunctionDeclarationDiagnosticConsumer(compiler.getDiagnostics().takeClient(), m_DesignEqualsImplementationFunctionDefinitionParser));
        return new DesignEqualsImplementationFunctionDeclarationAstConsumer(&compiler.getASTContext(), m_DesignEqualsImplementationFunctionDefinitionParser);
    }
private:
    DesignEqualsImplementationFunctionDeclarationParser *m_DesignEqualsImplementationFunctionDefinitionParser;
};

DesignEqualsImplementationFunctionDeclarationParser::DesignEqualsImplementationFunctionDeclarationParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend, QObject *parent)
    : QObject(parent)
    , m_HasUnrecoverableSyntaxError(false)
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
            m_HasUnrecoverableSyntaxError = true;
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
    while(!clang::tooling::runToolOnCode(new DesignEqualsImplementationFunctionDeclarationAstFrontendAction(this), functionDeclarationStdString))
    {
        if(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.isEmpty())
        {
            emit e("syntax error");
            m_HasUnrecoverableSyntaxError = true;
            return;
        }
        mutableFunctionDeclaration.prepend("typedef int " + m_UnknownTypesDetectedInLastRunToolOnCodeIteration.at(0) + ";\n"); //just one at a time, since the next one might be the same unknown type (right?)
        m_NewTypesSeenInFunctionDeclaration.append(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.at(0));
        m_UnknownTypesDetectedInLastRunToolOnCodeIteration.clear();
        m_ParsedFunctionArguments.clear();
        functionDeclarationStdString = mutableFunctionDeclaration.toStdString();
    }
    //success
}
bool DesignEqualsImplementationFunctionDeclarationParser::hasUnrecoverableSyntaxError() const
{
    return m_HasUnrecoverableSyntaxError;
}
QString DesignEqualsImplementationFunctionDeclarationParser::parsedFunctionName() const
{
    //TODOreq: error out if zero or two+ function declarations were seen
    return m_ParsedFunctionName;
}
QList<QString> DesignEqualsImplementationFunctionDeclarationParser::newTypesSeenInFunctionDeclaration() const
{
    return m_NewTypesSeenInFunctionDeclaration;
}
QList<MethodArgumentTypedef> DesignEqualsImplementationFunctionDeclarationParser::parsedFunctionArguments() const
{
    return m_ParsedFunctionArguments;
}
