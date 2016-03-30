#include "libclangfunctiondeclarationparser.h"

#include "libclangpch.h"
using namespace clang;

class LibClangFunctionDeclarationParserRecursiveAstVisitor : public RecursiveASTVisitor<LibClangFunctionDeclarationParserRecursiveAstVisitor>
{
public:
    explicit LibClangFunctionDeclarationParserRecursiveAstVisitor(ASTContext *context, LibClangFunctionDeclarationParser *libClangFunctionDeclarationParser)
        : m_Context(context)
        , m_LibClangFunctionDeclarationParser(libClangFunctionDeclarationParser)
    { }
    QualType recursivelyDereferenceToUltimatePointeeTypeAndDiscardQualifiersAndReferenceAmpersand(QualType qualifiedType) //TODOoptional: ask on clang website what proper way to do this is. there might be 'qualifiers' I'm forgetting to strip off
    {
        while(qualifiedType->isPointerType())
            qualifiedType = qualifiedType->getPointeeType();
        return qualifiedType.getUnqualifiedType().getNonReferenceType();
    }
    bool VisitFunctionDecl(FunctionDecl *functionDecl)
    {
        ++m_LibClangFunctionDeclarationParser->m_NumEncounteredFunctionDeclarations;
        m_LibClangFunctionDeclarationParser->m_ParsedFunctionName = QString::fromStdString(functionDecl->getNameAsString());
        //TO DOnereq(can't instantiate internal clang types eaisly): should i config a DesignEqualsImplementationMethod (rename to function?) or should i pass around the clang internal types? I'm leaning towards "depend on clang as little as possible (especially if i want to have a compile time switch to not even use it)", even though admittedly I'll be reinventing the wheel here and there (like I already am/have :-P). I'm still iffy on how I'm going to handle a variable "how the user typed it" (const SomeClass &blah -- etc) vs "underlying type" (SomeClass)... but I'm damn glad clang has allowed me to DETERMINE the underlying class :)
        unsigned paramCount = functionDecl->getNumParams();
        for(unsigned i = 0; i < paramCount; ++i)
        {
            const ParmVarDecl *currentParam = functionDecl->getParamDecl(i);
            QualType currentParamQualType = currentParam->getType(); //tempted to hang onto the QualType itself, but idfk how to deserialize/load back into one later on :(
            ParsedTypeInstance functionArgument;
            functionArgument.QualifiedType = QString::fromStdString(currentParamQualType.getAsString());
            functionArgument.NonQualifiedType = QString::fromStdString(recursivelyDereferenceToUltimatePointeeTypeAndDiscardQualifiersAndReferenceAmpersand(currentParamQualType).getAsString());
            functionArgument.Name = QString::fromStdString(currentParam->getNameAsString());

            if(m_LibClangFunctionDeclarationParser->m_NewTypesSeenInFunctionDeclaration.contains(functionArgument.NonQualifiedType))
            {
                //TODOlater (pending a refactor): I should make m_NewTypesSeenInFunctionDeclaration private and callers use this enum, but however it's still useful here/now to get to this block. it still deserves to exist, just privately
                functionArgument.ParsedTypeInstanceCategory = ParsedTypeInstance::Unknown;
            }
            else if(m_LibClangFunctionDeclarationParser->m_KnownTypesToTypedefPrepend.contains(functionArgument.NonQualifiedType))
            {
                functionArgument.ParsedTypeInstanceCategory = ParsedTypeInstance::KnownTypeButNotABuiltIn;
            }
            else //deductive reasoning yo: else if(currentParamQualType->isBuiltinType()) //we have to check this AFTER checking KnownToDefinedEqualsExcludingBuiltIns because oure 'known' types would be considered builtins by clang since it works on the canonical type
            {
                functionArgument.ParsedTypeInstanceCategory = ParsedTypeInstance::BuiltIn;
            }

            m_LibClangFunctionDeclarationParser->m_ParsedFunctionArguments.append(functionArgument);
        }
        return true;
    }
private:
    ASTContext *m_Context;
    LibClangFunctionDeclarationParser *m_LibClangFunctionDeclarationParser;
};

class LibClangFunctionDeclarationParserAstConsumer : public clang::ASTConsumer
{
public:
    explicit LibClangFunctionDeclarationParserAstConsumer(ASTContext *context, LibClangFunctionDeclarationParser *libClangFunctionDeclarationParser)
        : m_Visitor(context, libClangFunctionDeclarationParser)
    { }
    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        m_Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    LibClangFunctionDeclarationParserRecursiveAstVisitor m_Visitor;
};

struct LibClangFunctionDeclarationParserDiagnosticConsumer : clang::DiagnosticConsumer
{
    std::unique_ptr<DiagnosticConsumer> m_Proxy;
    LibClangFunctionDeclarationParserDiagnosticConsumer(DiagnosticConsumer *previous, LibClangFunctionDeclarationParser *libClangFunctionDeclarationParser)
        : m_Proxy(previous)
        , m_LibClangFunctionDeclarationParser(libClangFunctionDeclarationParser)
    { }
    void BeginSourceFile(const clang::LangOptions& langOpts, const clang::Preprocessor* preProcessor = 0) { m_Proxy->BeginSourceFile(langOpts, preProcessor); }
    void clear(){ m_Proxy->clear(); }
    void EndSourceFile() { m_Proxy->EndSourceFile(); }
    void finish() { m_Proxy->finish(); }
    void HandleDiagnostic(clang::DiagnosticsEngine::Level diagLevel, const clang::Diagnostic &diagnosticInfo)
    {
        if(diagLevel == DiagnosticsEngine::Error && (diagnosticInfo.getID() == clang::diag::err_unknown_typename || diagnosticInfo.getID() == clang::diag::err_unknown_typename_suggest) && diagnosticInfo.getNumArgs() > 0)
        {
            m_LibClangFunctionDeclarationParser->m_UnknownTypesDetectedInLastRunToolOnCodeIteration.append(QString::fromStdString(diagnosticInfo.getArgIdentifier(0)->getName().str()));
        }
        DiagnosticConsumer::HandleDiagnostic(diagLevel, diagnosticInfo);
        m_Proxy->HandleDiagnostic(diagLevel, diagnosticInfo);
    }
private:
    LibClangFunctionDeclarationParser *m_LibClangFunctionDeclarationParser;
};

class LibClangFunctionDeclarationParserAstFrontendAction : public clang::ASTFrontendAction
{
public:
    LibClangFunctionDeclarationParserAstFrontendAction(LibClangFunctionDeclarationParser *libClangFunctionDeclarationParser)
        : m_LibClangFunctionDeclarationParser(libClangFunctionDeclarationParser)
    { }
    virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef InFile)
    {
        Q_UNUSED(InFile)
        compiler.getDiagnostics().setClient(new LibClangFunctionDeclarationParserDiagnosticConsumer(compiler.getDiagnostics().takeClient(), m_LibClangFunctionDeclarationParser));
        return new LibClangFunctionDeclarationParserAstConsumer(&compiler.getASTContext(), m_LibClangFunctionDeclarationParser);
    }
private:
    LibClangFunctionDeclarationParser *m_LibClangFunctionDeclarationParser;
};
//lenient because it allows the "void " return type to be missing (but if return type is specified, it must be void), allows the parenthesis to be missing, and allows the semi-colon to be missing. in the simplest case, typing the signal/slot name alone is sufficient (i can't remember why i even parse in that case, but fuck it :-P... one code path is one code path)
LibClangFunctionDeclarationParser::LibClangFunctionDeclarationParser()
{
    reset();
}
LibClangFunctionDeclarationParser::LibClangFunctionDeclarationParser(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend)
{
    parseFunctionDeclaration(functionDeclaration, knownTypesToTypedefPrepend);
}
void LibClangFunctionDeclarationParser::reset()
{
    m_NumEncounteredFunctionDeclarations = 0;
    m_HasError = false;
    m_MostRecentError.clear();
    m_NewTypesSeenInFunctionDeclaration.clear();
    m_ParsedFunctionArguments.clear();
    m_ParsedFunctionName.clear();
    m_UnknownTypesDetectedInLastRunToolOnCodeIteration.clear();
}
void LibClangFunctionDeclarationParser::parseFunctionDeclaration(const QString &functionDeclaration, const QList<QString> &knownTypesToTypedefPrepend)
{
    reset();
    m_KnownTypesToTypedefPrepend = knownTypesToTypedefPrepend;

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
    while(!clang::tooling::runToolOnCode(new LibClangFunctionDeclarationParserAstFrontendAction(this), functionDeclarationStdString))
    {
        if(m_UnknownTypesDetectedInLastRunToolOnCodeIteration.isEmpty())
        {
            m_MostRecentError = "libclang reported a syntax error. Check stderr for the details";
            m_HasError = true;
            return;
        }
        Q_FOREACH(const QString &unknownType, m_UnknownTypesDetectedInLastRunToolOnCodeIteration)
        {
            if(m_NewTypesSeenInFunctionDeclaration.contains(unknownType))
                continue;
            mutableFunctionDeclaration.prepend("typedef int " + unknownType + ";\n"); //just one at a time, since the next one might be the same unknown type (right?)
            m_NewTypesSeenInFunctionDeclaration.append(unknownType);
        }
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
bool LibClangFunctionDeclarationParser::hasError() const
{
    return m_HasError;
}
//only valid if has error
QString LibClangFunctionDeclarationParser::mostRecentError() const
{
    return m_MostRecentError;
}
QString LibClangFunctionDeclarationParser::parsedFunctionName() const
{
    return m_ParsedFunctionName;
}
QList<QString> LibClangFunctionDeclarationParser::newTypesSeenInFunctionDeclaration() const
{
    return m_NewTypesSeenInFunctionDeclaration;
}
QList<ParsedTypeInstance> LibClangFunctionDeclarationParser::parsedFunctionArguments() const
{
    return m_ParsedFunctionArguments;
}
