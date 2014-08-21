#include "functiondecomposerwithnewtypedetector.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "clang/Sema/SemaDiagnostic.h"

using namespace clang;

class RecursiveFunctionDecomposerVisitor : public RecursiveASTVisitor<RecursiveFunctionDecomposerVisitor>
{
public:
    explicit RecursiveFunctionDecomposerVisitor(ASTContext *context, FunctionDecomposerWithNewTypeDetector *functionDecomposerWithNewTypeDetector)
        : m_Context(context)
        , m_FunctionDecomposerWithNewTypeDetector(functionDecomposerWithNewTypeDetector)
    { }
    bool VisitDecl(Decl *declaration)
    {
        if(FunctionDecl *functionDecl = dyn_cast<FunctionDecl>(declaration))
        {
            //TODOreq: should i config a DesignEqualsImplementationMethod (rename to function?) or should i pass around the clang internal types? I'm leaning towards "depend on clang as little as possible (especially if i want to have a compile time switch to not even use it)", even though admittedly I'll be reinventing the wheel here and there (like I already am/have :-P). I'm still iffy on how I'm going to handle a variable "how the user typed it" (const SomeClass &blah -- etc) vs "underlying type" (SomeClass)... but I'm damn glad clang has allowed me to DETERMINE the underlying class :)
        }
        return true;
    }
private:
    ASTContext *m_Context;
    FunctionDecomposerWithNewTypeDetector *m_FunctionDecomposerWithNewTypeDetector;
};

class FunctionDecomposerConsumer : public clang::ASTConsumer
{
public:
    explicit FunctionDecomposerConsumer(ASTContext *context, FunctionDecomposerWithNewTypeDetector *functionDecomposerWithNewTypeDetector)
        : m_Visitor(context, functionDecomposerWithNewTypeDetector)
    { }
    virtual void HandleTranslationUnit(clang::ASTContext &context)
    {
        m_Visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
private:
    RecursiveFunctionDecomposerVisitor m_Visitor;
};

struct FunctionDecomposerDiagnosticConsumer : clang::DiagnosticConsumer
{
    llvm::OwningPtr<DiagnosticConsumer> m_Proxy;
    FunctionDecomposerDiagnosticConsumer(DiagnosticConsumer *previous, FunctionDecomposerWithNewTypeDetector *functionDecomposerWithNewTypeDetector)
        : m_Proxy(previous)
        , m_FunctionDecomposerWithNewTypeDetector(functionDecomposerWithNewTypeDetector)
    { }
    void BeginSourceFile(const clang::LangOptions& langOpts, const clang::Preprocessor* preProcessor = 0) { m_Proxy->BeginSourceFile(langOpts, preProcessor); }
    void clear(){ m_Proxy->clear(); }
    void EndSourceFile() { m_Proxy->EndSourceFile(); }
    void finish() { m_Proxy->finish(); }
    void HandleDiagnostic(clang::DiagnosticsEngine::Level diagLevel, const clang::Diagnostic &diagnosticInfo)
    {
        if(diagLevel == DiagnosticsEngine::Error && (diagnosticInfo.getID() == clang::diag::err_unknown_typename || diagnosticInfo.getID() == clang::diag::err_unknown_typename_suggest) && diagnosticInfo.getNumArgs() > 0)
        {
            //llvm::outs() << "Unknown type detected: " << diagnosticInfo.getArgIdentifier(0)->getName().str() << "\n";
            m_FunctionDecomposerWithNewTypeDetector->m_UnknownTypesDetectedThisIteration.append(QString::fromStdString(diagnosticInfo.getArgIdentifier(0)->getName().str())); //*crosses fingers that libclang isn't multi-threaded behind the scenes* (a simple mutex lock would save the day)
        }
        DiagnosticConsumer::HandleDiagnostic(diagLevel, diagnosticInfo);
        m_Proxy->HandleDiagnostic(diagLevel, diagnosticInfo);
    }
private:
    FunctionDecomposerWithNewTypeDetector *m_FunctionDecomposerWithNewTypeDetector;
};

class FunctionDecomposerFrontendAction : public clang::ASTFrontendAction
{
public:
    FunctionDecomposerFrontendAction(FunctionDecomposerWithNewTypeDetector *functionDecomposerWithNewTypeDetector)
        : m_FunctionDecomposerWithNewTypeDetector(functionDecomposerWithNewTypeDetector)
    { }
    virtual clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &compiler, llvm::StringRef InFile)
    {
        Q_UNUSED(InFile)
        compiler.getDiagnostics().setClient(new FunctionDecomposerDiagnosticConsumer(compiler.getDiagnostics().takeClient(), m_FunctionDecomposerWithNewTypeDetector));
        return new FunctionDecomposerConsumer(&compiler.getASTContext(), m_FunctionDecomposerWithNewTypeDetector);
    }
private:
    FunctionDecomposerWithNewTypeDetector *m_FunctionDecomposerWithNewTypeDetector;
};

//TODOreq: error out on syntax error, error out if more than one function declaration, error out if return type isn't void (since signals/slots for now)
//TODOreq: append semicolon if missing, prepend "void " if missing (but this conflicts with if they do an int return type for example)
FunctionDecomposerWithNewTypeDetector::FunctionDecomposerWithNewTypeDetector(QObject *parent)
    : QObject(parent)
{ }
void FunctionDecomposerWithNewTypeDetector::parseFunctionDeclaration(const QString &functionDeclaration)
{
    QString userInputFunctionDeclarationFixed = functionDeclaration;
    if(!functionDeclaration.trimmed().endsWith(";"))
    {
        userInputFunctionDeclarationFixed.append(";");
    }
    QString functionDeclarationTemp = userInputFunctionDeclarationFixed;
    std::string functionDeclarationStdString = functionDeclarationTemp.toStdString();
    m_UnknownTypesDetectedThisIteration.clear();
    m_TypesToBeCreated.clear();
    while(!clang::tooling::runToolOnCode(new FunctionDecomposerFrontendAction(this), functionDeclarationStdString))
    {
        if(m_UnknownTypesDetectedThisIteration.isEmpty())
        {
            emit syntaxErrorDetected();
            return;
        }
        functionDeclarationTemp.prepend("typedef int " + m_UnknownTypesDetectedThisIteration.at(0) + "; "); //just one at a time, since the next one might be the same unknown type
        m_TypesToBeCreated.append(m_UnknownTypesDetectedThisIteration.at(0));
        m_UnknownTypesDetectedThisIteration.clear();
        functionDeclarationStdString = functionDeclarationTemp.toStdString();
    }
    emit o("success");
    Q_FOREACH(const QString &currentNewClass, m_TypesToBeCreated)
    {
        emit o("Create class: " + currentNewClass);
    }
    //emit o(function name and args and blah blah blah
}
