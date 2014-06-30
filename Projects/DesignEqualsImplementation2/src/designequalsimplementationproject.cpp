#include "designequalsimplementationproject.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDataStream>

#define DesignEqualsImplementationProject_SERIALIZATION_VERSION 1

#define DesignEqualsImplementationProject_QDS(direction, qds, project) \
qds direction serializationVersion; \
qds direction project.Name; \
qds direction project.Classes; \
qds direction project.UseCases;

//new
DesignEqualsImplementationProject::DesignEqualsImplementationProject(QObject *parent)
    : QObject(parent)
{ }
//open
DesignEqualsImplementationProject::DesignEqualsImplementationProject(const QString &existingProjectFilePath, QObject *parent)
    : QObject(parent)
{
    //TODOreq
}
DesignEqualsImplementationProject::~DesignEqualsImplementationProject()
{
    qDeleteAll(Classes);
    qDeleteAll(UseCases);
}
bool DesignEqualsImplementationProject::savePrivate(const QString &projectFilePath)
{

    return true;
}
//TODOreq: atomic project source generation. if it fails, no files are written/modified
bool DesignEqualsImplementationProject::generateSourceCodePrivate(const QString &destinationDirectoryPath)
{
    //rambling: so basically our "Classes" member contains only the CORE classes of the project/design, but when we generate the source code we are still going to need to generate glue classes that own/instantiate/connect those core classes

    if(QFile::exists(destinationDirectoryPath))
    {
        emit e("destination directory must not exist: " + destinationDirectoryPath); //temp (very safe) fuck it for nows
        return false;
    }
    QDir destinationDir(destinationDirectoryPath);
    if(!destinationDir.mkpath(destinationDirectoryPath))
    {
        emit e("failed to make destination dir: " + destinationDirectoryPath);
        return false;
    }

    //TODOreq: proper these hardcoded UI hacks
    //hardcoded main.cpp, pro, and *cli.h/cpp
    if(!tempGenerateHardcodedUiFiles(destinationDirectoryPath))
    {
        emit e("failed to generate temp hardcoded ui files in dir: " + destinationDirectoryPath);
        return false;
    }

#if 0
    Q_FOREACH(DesignEqualsImplementationClass *designEqualsImplementationClass, Classes)
    {
        if(!designEqualsImplementationClass->generateSourceCode(destinationDirectoryPath)) //bleh, async model breaks down here. can't be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e("failed to generate source for: " + designEqualsImplementationClass->Name);
            return false;
        }
    }
#endif

    //Q_FOREACH -- modifying use cases modifies the associated classes implicitly, so in theory there is no source to generate for use cases. HOWEVER, as of now the use cases are more focused on and contain the "full references" of the project as a whole, so generating (rather, trying to generate :-P) THAT should implicitly generate all classes (and more importantly, all parts of the class referenced in a use case)

    //TODOreq: proper design just jumped out at me: iterate the use cases at source generation time (NOW), but have the use cases do nothing more than populate the class diagram (ok ok, this is actually something that should happen any time class diagram view is lookeded at (or anything in the project changes if you don't do lazy loading)), THEN after use case iteratinos have spun class diagrams, outputting the classes by themselves is a breeze. however for now i'm going to try to just stick with focusing on use cases specifically, because i can't figure out where in a "Class" the "Ordered Use Case Events" would go

    //TODOreq: just like a use case needed an initial slot, so too does a project need an initial use case. It makes sense that if there is only 1x use case, that it is the "default" when invoked via CLI. If, for example, there are 2+ use cases, then the CLI wouldn't know which to use as the default (we could opt towards using the "first added", but that is dumb imo and i think actually that a "project::setDefaultUseCase" (already added) makes sense. Without setting said default, the CLI version should do NOTHING [unless cli arg switches indicate a use case], and the GUI version has the option of doing none or all. (two line edits, two push buttons, one shared qlineedit for output OR SOMETHING)

    Q_FOREACH(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, UseCases)
    {
        if(!designEqualsImplementationUseCase->generateSourceCode(destinationDirectoryPath)) //bleh, async model breaks down here. can't be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e("failed to generate source for: " + designEqualsImplementationUseCase->Name);
            return false;
        }
    }

    return true;
}
bool DesignEqualsImplementationProject::tempGenerateHardcodedUiFiles(const QString &destinationDirectoryPath)
{
    //main.cpp
    QString mainCppFilePath = destinationDirectoryPath + "main.cpp";
    QFile mainCppFile(mainCppFilePath, this);
    if(!mainCppFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + mainCppFilePath);
        return false;
    }
    QTextStream mainCppTextStream(&mainCppFile);
    mainCppTextStream << "#include <QCoreApplication>" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << "#include \"simpleasynclibraryslotinvokeandsignalresponsecli.h\"" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << "int main(int argc, char *argv[])" << endl;
    mainCppTextStream << "{" << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "QCoreApplication a(argc, argv);" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << Name << "Cli cli;" << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_UNUSED(cli)" << endl;
    mainCppTextStream << endl;
    mainCppTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "return a.exec();" << endl;
    mainCppTextStream << "}" << endl;

    //*cli.h
    QString cliHFilePath = destinationDirectoryPath + Name.toLower() + "cli.h";
    QFile cliHFile(cliHFilePath, this);
    if(!cliHFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + cliHFilePath);
        return false;
    }
    QTextStream cliHTextStream(&cliHFile);
    QString headerDoubleIncludeGuardDefine = Name.toUpper() + "CLI_H";
    cliHTextStream << "#ifndef " << headerDoubleIncludeGuardDefine << endl;
    cliHTextStream << "#define " << headerDoubleIncludeGuardDefine << endl;
    cliHTextStream << endl;
    cliHTextStream << "#include <QObject>" << endl;
    cliHTextStream << "#include <QTextStream>" << endl; //wat
    cliHTextStream << endl;
    QString cliClassName = Name + "Cli";
    cliHTextStream << "class " << cliClassName << " : public QObject" << endl;
    cliHTextStream << "{" << endl;
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_" << "OBJECT" << endl;
    cliHTextStream << "public:" << endl;
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << cliClassName << "(QObject *parent = 0);" << endl;
    cliHTextStream << "private:" << endl;
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "QTextStream m_StdOut;";

    //TODOreq: generic cli class
    //private slots:
    //  void handleFooSignal(bool success);

    cliHTextStream << "};" << endl;
    cliHTextStream << endl;
    cliHTextStream << "#endif // " << headerDoubleIncludeGuardDefine << endl;

    //*cli.cpp
    QString cliCppFilePath = destinationDirectoryPath + Name.toLower() + "cli.cpp";
    QFile cliCppFile(cliCppFilePath, this);
    if(!cliCppFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + cliCppFilePath);
        return false;
    }
    QTextStream cliCppTextStream(&cliCppFile);

#if 0 //TODOreq: oops, seems *cli.h/cpp aren't as static as I thought. I think when generating I need to generate the least dependent class first
#include "simpleasynclibraryslotinvokeandsignalresponsecli.h"

#include <QCoreApplication>

#include "../../foo.h"

//UI is not being factored into designEquals at this point, so this class is not even trying to be elegant/etc
SimpleAsyncLibrarySlotInvokeAndSignalResponseCli::SimpleAsyncLibrarySlotInvokeAndSignalResponseCli(QObject *parent)
    : QObject(parent)
    , m_StdOut(stdout)
{
    Foo *foo = new Foo(this);
    connect(foo, SIGNAL(fooSignal(bool)), this, SLOT(handleFooSignal(bool)));
    QMetaObject::invokeMethod(foo, "fooSlot", Q_ARG(QString, QString("test"))); //or could be a permanent connection with corresponding this.fooSlotInvocationRequested signal to start everything off
}
void SimpleAsyncLibrarySlotInvokeAndSignalResponseCli::handleFooSignal(bool success)
{
    m_StdOut << QString("fooSignal indicated success = " + success ? "true" : "false") << endl;
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}

#endif

return true;
}
void DesignEqualsImplementationProject::save(const QString &projectFilePath)
{
    emit saved(savePrivate(projectFilePath));
}
//TODOreq: overwrite warnings, backing up user modifications, putting "do not edit this file" warnings, etc etc
//TODOreq: enum to tell what kind of source to generate (cli, gui, lib, potentially 'web' eventually)
void DesignEqualsImplementationProject::generateSourceCode(const QString &destinationDirectoryPath)
{
    emit sourceCodeGenerated(generateSourceCodePrivate(appendSlashIfNeeded(destinationDirectoryPath)));
}
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion = DesignEqualsImplementationProject_SERIALIZATION_VERSION;
    DesignEqualsImplementationProject_QDS(<<, out, project);
    return out;
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion;
    DesignEqualsImplementationProject_QDS(>>, in, project);
    return in;
}
