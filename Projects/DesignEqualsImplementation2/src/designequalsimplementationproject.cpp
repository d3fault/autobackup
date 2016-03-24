#include "designequalsimplementationproject.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDataStream>
#include <QProcess>
#include <QDateTime>

#include "designequalsimplementationprojectgenerator.h"
#include "designequalsimplementationchunkofrawcppstatements.h"
#include "designequalsimplementationimplicitlyshareddatatype.h"

#define DesignEqualsImplementationProject_SERIALIZATION_VERSION 1

#define DesignEqualsImplementationProject_QDS(direction, qds, project) \
qds direction serializationVersion; \
qds direction project.Name; \
qds direction project.m_Classes; \
qds direction project.m_UseCases; \
qds direction project.m_DefinedElsewhereTypes; \
return qds;

DesignEqualsImplementationProject::DesignEqualsImplementationProject(QObject *parent)
    : QObject(parent)
{ }
DesignEqualsImplementationProject::~DesignEqualsImplementationProject()
{
    qDeleteAll(m_UseCases);
}
DesignEqualsImplementationClass *DesignEqualsImplementationProject::createNewClass(const QString &newClassName, const QPointF &classPositionInGraphicsScene)
{
    DesignEqualsImplementationClass *newClass = new DesignEqualsImplementationClass(this, this);
    newClass->Name = newClassName;
    newClass->Position = classPositionInGraphicsScene;
    addType(newClass);
    return newClass;
}
DesignEqualsImplementationImplicitlySharedDataType *DesignEqualsImplementationProject::createNewImplicitlySharedDataType(const QString &newImplicitlySharedDataTypeName, const QPointF &positionInGraphicsScene)
{
    DesignEqualsImplementationImplicitlySharedDataType *newDataType = new DesignEqualsImplementationImplicitlySharedDataType(this, this);
    newDataType->Name = newImplicitlySharedDataTypeName;
    newDataType->Position = positionInGraphicsScene;
    addType(newDataType);
    return newDataType;
}
QList<Type*> DesignEqualsImplementationProject::allKnownTypes() const
{
    return m_AllKnownTypes;
}
QList<QString> DesignEqualsImplementationProject::allKnownTypesNames() const
{
    QList<QString> ret;
    Q_FOREACH(Type *type, allKnownTypes())
        ret << type->Name;
    return ret;
}
Type *DesignEqualsImplementationProject::getOrCreateTypeFromName(const QString &nonQualifiedTypeName)
{
    Q_FOREACH(Type *currentType, allKnownTypes())
    {
        if(currentType->Name == nonQualifiedTypeName)
            return currentType;
    }
    QString warningMessage("WARNING: getorCreateTypeFromName could not 'get' the type, so one was 'created' via 'noteDefinedElsewhereType'. This is probably a bug (but is failing gracefully). The type name is: " + nonQualifiedTypeName);
    std::string warningMessageStd = warningMessage.toStdString();
    qWarning(warningMessageStd.c_str()); //TODOreq: delete this method and 'proper' the Types database system! This method just saves me a bit of refactoring so whatever...
    return noteDefinedElsewhereType(nonQualifiedTypeName);
}
void DesignEqualsImplementationProject::addType(Type *newType)
{
    //TODOoptional: make all appends to m_AllKnownTypes use this method, then add a "typeAdded" signal. ClassDiagramScene would be a listener of that signal, but I'm not sure whether or not I should visualize "types defined elsewhere" (perhaps that should be a dynamic user preference)

    connect(newType, SIGNAL(e(QString)), this, SIGNAL(e(QString)));
    m_AllKnownTypes.append(newType);
    emit typeAdded(newType);
}
QList<DesignEqualsImplementationClass*> DesignEqualsImplementationProject::classes()
{
    QList<DesignEqualsImplementationClass *> ret;
    Q_FOREACH(Type *currentType, allKnownTypes())
    {
        DesignEqualsImplementationClass *maybe = qobject_cast<DesignEqualsImplementationClass*>(currentType);
        if(maybe)
            ret << maybe;
    }
    return ret;
}
#if 0
DesignEqualsImplementationClassInstance* DesignEqualsImplementationProject::createTopLevelClassInstance(DesignEqualsImplementationClass *classToMakeTopLevelInstanceOf)
{
    DesignEqualsImplementationClassInstance *classInstance = new DesignEqualsImplementationClassInstance(classToMakeTopLevelInstanceOf, 0, 0, "topLevel_" + classToMakeTopLevelInstanceOf->Name); //TODOreq: auto increment for top level auto vars
    m_TopLevelClassInstances.append(classInstance);
    return classInstance;
}
#endif
#if 0 //TODOinstancing
QList<DesignEqualsImplementationClassInstance*> DesignEqualsImplementationProject::topLevelClassInstances() //TODOreq: delete? maintain only list of use cases? it used to be a list of instances btw, before the refactor to 'this'/use-cases-root-class-lifeline
{
    return m_TopLevelClassInstances;
}
#endif
void DesignEqualsImplementationProject::addUseCase(DesignEqualsImplementationUseCase *newUseCase)
{
    //connect(newUseCase, SIGNAL(e);
    m_UseCases.append(newUseCase);
    emit useCaseAdded(newUseCase);
}
QList<DesignEqualsImplementationUseCase *> DesignEqualsImplementationProject::useCases()
{
    return m_UseCases;
}
DefinedElsewhereType *DesignEqualsImplementationProject::noteDefinedElsewhereType(const QString &definedElsewhereType)
{
    //note: at first i thought i'd need to 'ensure a defined elsewhere type is never noted more than once', BUT currently i am always building allKnownTypes using the defined elsewhere types, which means it would be impossible for me to detect a defined elsewhere type as a new type (which is prerequisite for me noting that it's defined elsewhere). still, a qset would de-dupe with ease if i ever can't make that guarantee in the future

    DefinedElsewhereType *newDefinedElsewhereType = new DefinedElsewhereType(this, this);
    newDefinedElsewhereType->Name = definedElsewhereType;
    m_AllKnownTypes.append(newDefinedElsewhereType); //TODOreq: [de-]serialize
    return newDefinedElsewhereType;
}
//TODOoptimization: vacuum types, since ones that used to be in project might not anymore. vacuum just before serialization imo
#if 0
QList<DefinedElsewhereType*> DesignEqualsImplementationProject::definedElsewhereTypes() const
{
    QList<DefinedElsewhereType*> ret;
    Q_FOREACH(Type *currentType, allKnownTypes())
    {
        DefinedElsewhereType *maybe = qobject_cast<DefinedElsewhereType*>(currentType);
        if(maybe)
            ret << maybe;
    }
    return m_DefinedElsewhereTypes;
}
#endif
int DesignEqualsImplementationProject::serializationTypeIdForType(Type *typeToReturnSerializationIdFor)
{
    return allKnownTypes().indexOf(typeToReturnSerializationIdFor);
#if 0
    int ret = m_Classes.indexOf(typeToReturnSerializationIdFor);
    if(ret > -1)
        return ret;

    //not in m_Classes, search defined elsewhere types
    ret = m_Classes.size();
#endif
}
Type *DesignEqualsImplementationProject::typeFromSerializedTypeId(int serializedTypeId)
{
    return allKnownTypes().at(serializedTypeId);
}
QString DesignEqualsImplementationProject::projectFileName()
{
    return Name + ".pro";
}
#if 0
void DesignEqualsImplementationProject::appendLineToTemporaryProjectGlueCode(const QString &line)
{
    m_TemporaryProjectGlueCodeLines.append(line);
}
#endif
bool DesignEqualsImplementationProject::writeTemporaryGlueCodeLines(const QString &destinationDirectoryPath)
{
    QString autoGlue = "AutoGlue_";
    QString autoGlueProjectName = autoGlue + Name;

    QString filenameToLowerWithoutExtension = autoGlue + Name;
    filenameToLowerWithoutExtension = filenameToLowerWithoutExtension.toLower();

    //autoglue_projectname.h
    QString projectGlueFilenameOnlyToLower = filenameToLowerWithoutExtension;
    QString projectGlueHeaderFilenameOnly = projectGlueFilenameOnlyToLower + ".h";
    QString projectGlueHeaderFilename = destinationDirectoryPath + projectGlueHeaderFilenameOnly;
    QFile projectGlueHeaderFile(projectGlueHeaderFilename);
    if(!projectGlueHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + projectGlueHeaderFilename);
        return false;
    }
    QTextStream projectGlueHeaderTextStream(&projectGlueHeaderFile);
    QString projectGlueHeadderGuard = Name.toUpper() + "_H";
    projectGlueHeaderTextStream << "#ifndef " << projectGlueHeadderGuard << endl;
    projectGlueHeaderTextStream << "#define " << projectGlueHeadderGuard << endl;
    projectGlueHeaderTextStream << endl;
    projectGlueHeaderTextStream << "#include <QObject>" << endl;
    projectGlueHeaderTextStream << endl;
    projectGlueHeaderTextStream << "class " << autoGlueProjectName << " : public QObject" << endl;
    projectGlueHeaderTextStream << "{" << endl;
    projectGlueHeaderTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "Q_OBJECT" << endl;
    projectGlueHeaderTextStream << "public:" << endl;
    projectGlueHeaderTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "explicit " << autoGlueProjectName << "(QObject *parent = 0);" << endl;
    projectGlueHeaderTextStream << "};" << endl;
    projectGlueHeaderTextStream << endl;
    projectGlueHeaderTextStream << "#endif // " << projectGlueHeadderGuard << endl;

    //autoglue_projectname.cpp
    QString projectGlueSourceFileName = filenameToLowerWithoutExtension + ".cpp";
    QFile projectGlueSourceFile(destinationDirectoryPath + projectGlueSourceFileName);
    if(!projectGlueSourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit e("failed to open for writing: " + projectGlueSourceFileName);
        return false;
    }
    QTextStream projectGlueSourceTextStream(&projectGlueSourceFile);
    projectGlueSourceTextStream << "#include \"" << projectGlueHeaderFilenameOnly << "\"" << endl;
    projectGlueSourceTextStream << endl;
    projectGlueSourceTextStream << autoGlueProjectName << "::" << autoGlueProjectName << "(QObject *parent)" << endl;
    projectGlueSourceTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << ": QObject(parent)" << endl;
    projectGlueSourceTextStream << "{" << endl;
    //  Foo *foo = new Foo(this);"
#if 0
    Q_FOREACH(const QString &currentTempGlueLine, m_TemporaryProjectGlueCodeLines)
    {
        projectGlueSourceTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << currentTempGlueLine << endl; //TODOoptional: IStatement type shiz
    }
#endif
    projectGlueSourceTextStream << "}" << endl;

    return true;
}
//TODOreq: atomic project source generation. if it fails, no files are written/modified
bool DesignEqualsImplementationProject::generateSourceCodePrivate(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, bool generateCppEditModeDelimitingComments, int *out_LineNumberToJumpTo_OrZeroIfNotApplicable, DesignEqualsImplementationClassSlot *slotWeWantLineNumberOf_OnlyWhenApplicable, int statementIndexOfSlotToGetLineNumberOf_OnlyWhenApplicable)
{
    //rambling: so basically our "Classes" member contains only the CORE classes of the project/design, but when we generate the source code we are still going to need to generate glue classes that own/instantiate/connect those core classes

    if(!allClassLifelinesInAllUseCasesInProjectHaveBeenAssignedInstances())
    {
        emit e("at least one class lifeline out of all your use cases in the project has not been assigned an instance"); //TODOoptional: tell them which one. TODOoptional: allow them to jump to it
        return false;
    }

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

    //TODOreq: should library generation include a .pro file? a .pri file? both? better solution in qbs? all of the above (default to which?)?

    DesignEqualsImplementationProjectGenerator projectGenerator(this, projectGenerationMode, destinationDirectoryPath, generateCppEditModeDelimitingComments, out_LineNumberToJumpTo_OrZeroIfNotApplicable, slotWeWantLineNumberOf_OnlyWhenApplicable, statementIndexOfSlotToGetLineNumberOf_OnlyWhenApplicable);
    connect(&projectGenerator, SIGNAL(e(QString)), this, SIGNAL(e(QString)));

#if 0 //TODOreq: merge with project generator
    if(projectGenerationMode != Library)
    {
        emit e("TODOreq: can only generate library for now");
        return false;

        //TODOreq: proper these hardcoded UI hacks
        //hardcoded main.cpp and *cli.h/cpp
        if(!tempGenerateHardcodedUiFiles(destinationDirectoryPath))
        {
            emit e("failed to generate temp hardcoded ui files in dir: " + destinationDirectoryPath);
            return false;
        }
    }
#endif

    //cleanupJitGeneratedLinesFromAPreviousGenerate();

    if(!projectGenerator.generateProjectFileAndWriteItToDisk()) //TODOreq: cli/lib/gui differences
    {
        emit e("failed to generate project file");
        return false;
    }

    //two steps because use cases append connect statements to class constructors before writing
    Q_FOREACH(DesignEqualsImplementationClass *designEqualsImplementationClass, classes())
    {
        if(!projectGenerator.processClassStep0declaringClassInProject(designEqualsImplementationClass))
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX ", class step 0, " + designEqualsImplementationClass->Name);
            return false;
        }
    }
    Q_FOREACH(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, useCases())
    {
        if(!projectGenerator.processUseCase(designEqualsImplementationUseCase)) //bleh, async model breaks down here. can't [easily (code generator *cough* could do it)] be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX ", use case: " + designEqualsImplementationUseCase->Name);
            return false;
        }
    }

#if 0 //TODOreq maybe use this somewhere else or again later idfk
    if(!writeTemporaryGlueCodeLines(destinationDirectoryPath))
    {
        emit e("Failed to write temporary glue code for project. Check above for errors");
        return false;
    }
#endif

#if 0
    Q_FOREACH(DesignEqualsImplementationClass *designEqualsImplementationClass, m_Classes)
    {
        //TODOreq: use case's exit signals should just be serialized/remembered. they are of design use, but of zero use when it comes to code generation (the signal is simply emitted). i was tempted to say "use case exit signals should generate temporary copies of the classes here, now with the exit signals inserted (temporary copies used to generate source code)", BUT as you can see that's wrong
        if(!projectGenerator.processClassStep1writingTheFile(designEqualsImplementationClass)) //bleh, async model breaks down here. can't be async _AND_ get a bool success value :(. also, TODOreq: maybe optional idfk, but the classes should maybe be organized into [sub-]directories instead of all being in the top most directory
        {
            emit e(DesignEqualsImplementationClass_FAILED_TO_GENERATE_SOURCE_PREFIX + ", class step 1, " + designEqualsImplementationClass->Name);
            return false;
        }
    }
#endif
    if(!projectGenerator.writeAllTypesToDisk())
    {
        emit e("failed to write project files to disk");
        return false;
    }

    //Q_FOREACH -- modifying use cases modifies the associated classes implicitly, so in theory there is no source to generate for use cases. HOWEVER, as of now the use cases are more focused on and contain the "full references" of the project as a whole, so generating (rather, trying to generate :-P) THAT should implicitly generate all classes (and more importantly, all parts of the class referenced in a use case)

    //TODOreq: proper design just jumped out at me: iterate the use cases at source generation time (NOW), but have the use cases do nothing more than populate the class diagram (ok ok, this is actually something that should happen any time class diagram view is lookeded at (or anything in the project changes if you don't do lazy loading)), THEN after use case iteratinos have spun class diagrams, outputting the classes by themselves is a breeze. however for now i'm going to try to just stick with focusing on use cases specifically, because i can't figure out where in a "Class" the "Ordered Use Case Events" would go

    //TODOreq: just like a use case needed an initial slot, so too does a project need an initial use case. It makes sense that if there is only 1x use case, that it is the "default" when invoked via CLI. If, for example, there are 2+ use cases, then the CLI wouldn't know which to use as the default (we could opt towards using the "first added", but that is dumb imo and i think actually that a "project::setDefaultUseCase" (already added) makes sense. Without setting said default, the CLI version should do NOTHING [unless cli arg switches indicate a use case], and the GUI version has the option of doing none or all. (two line edits, two push buttons, one shared qlineedit for output OR SOMETHING)

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
    cliHTextStream << DESIGNEQUALSIMPLEMENTATION_TAB << "QTextStream m_StdOut;" << endl;

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

#if 0 //TODOreq: oops, seems *cli.h/cpp aren't as static as I thought. I think when generating I need to generate the least dependent class first. Should I do dependency resolution (Bar depends on (is a child of) Foo, but Foo depends on nobody, therefore Foo goes in this Cli class), or should I just choose whatever class+slot is used as the [default] entry point (which, at the time of writing, is simply the first added use case event)? The latter is obviously easier, BUT I think I'm going to need to do some kind of dependency resolution anyways to determine the optimal (read: "latest as possible" (but not doing "jit instantiation" for now)) position for Bar to be instantiated (Foo's constructor in this case, BUT the app would work the exact fucking same if Foo/Bar were neighbors (and in that case, Bar would need to be in this Cli class's constructor)
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
bool DesignEqualsImplementationProject::allClassLifelinesInAllUseCasesInProjectHaveBeenAssignedInstances()
{
    Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, useCases())
    {
        if(!currentUseCase->allClassLifelinesInUseCaseHaveBeenAssignedInstances())
        {
            emit e("a class class lifeline in use case '" + currentUseCase->Name + "' was not assigned an instance");
            return false;
        }
    }
    return true;
}
#if 0
void DesignEqualsImplementationProject::cleanupJitGeneratedLinesFromAPreviousGenerate()
{
    m_TemporaryProjectGlueCodeLines.clear();
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_Classes)
    {
        currentClass->cleanupJitGeneratedLinesFromAPreviousGenerate();
    }
}
#endif
void DesignEqualsImplementationProject::emitAllClassesAndUseCasesInProject()
{
#if 0
    //TODOreq: the signals will be emitted twice when LOADING a file (once when each individual deserialized 'thing' is added, and once again now). i'm not sure if 'some' of the later ones in the first batch might in fact be received, or if all of the first batch are definitely discarded (ui not listening yet)
    Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, m_UseCases)
    {
        emit useCaseAdded(currentUseCase);
    }
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, classes())
    {
        emit classAdded(currentClass);
    }
#endif
}
void DesignEqualsImplementationProject::handleAddUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position)
{
    switch(umlItemType)
    {
    case DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS:
    {
        createNewClass("NewClass1" /*TODOreq: auto incrementing (but they'll likely change it anyways)*/, position);
    }
        break;
    case DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_CLASS_INTERFACE:
    {
        //TODOreq
    }
        break;
    case DESIGNEQUALSIMPLEMENTATION_MIME_DATA_VALUE_UML_IMPLICITLY_SHARED_DATA_TYPE:
    {
        createNewImplicitlySharedDataType("NewImplicitlySharedDataType1", position); //TODOreq: auto incrementing
    }
        break;
    }
}
void DesignEqualsImplementationProject::handleNewUseCaseRequested()
{
    DesignEqualsImplementationUseCase *useCase = new DesignEqualsImplementationUseCase(this, this);
    useCase->Name = Name + " - Use Case 1"; //TODOreq: auto incrementing
    addUseCase(useCase);
}
//TODOoptional: don't require CLOSING qt creator before sucking the changes back in. if they save-all in qt creator and then press a button in d=i, we could THEN suck in the changes [using the exact same code already here]. HOWEVER, going to edit C++ mode _AGAIN_ should then overwrite the existing project, which is fine because qt creator can handle it (it just pops up "want to load the changes?" and you press yes)
void DesignEqualsImplementationProject::handleEditCppModeRequested(Type *designEqualsImplementationClass, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot, int statementIndexOfSlot)
{
    QStringList possibleQtCreatorPaths;
    possibleQtCreatorPaths << "/usr/bin/qtcreator" << "/home/d3fault/Qt5.4.1/Tools/QtCreator/bin/qtcreator" << "/home/user/Qt5.2.0/Tools/QtCreator/bin/qtcreator"; //TODOreq: cli arg and/or qsettings value and/or "sensible defaults" on per-OS basis
    QString existingQtCreator;
    Q_FOREACH(const QString &possibleQtCreatorPath, possibleQtCreatorPaths)
    {
        if(QFile::exists(possibleQtCreatorPath))
        {
            existingQtCreator = possibleQtCreatorPath;
            break;
        }
    }
    if(existingQtCreator.isEmpty())
    {
        emit e("Qt Creator not found!");
        return;
    }

    //TODOreq: explain in messagebox what's about to happen (maybe give cancel button), and explain the rules about the comments delimeters (also: "don't show this messagebox again" checkbox). should probably lock the entire application while qtcreator is running (however, waitForFinished() pretty much does this already), i think qapplication has a method for that

    //TODOreq: customizeable temp dir shit. TODOreq: raw C++ in constructor/destructor
    QString cppEditTemporariesPath("/run/shm/designEqualsImplementation-CppEditTemporariesAt_" + QString::number(QDateTime::currentMSecsSinceEpoch()) + QDir::separator());
    QScopedPointer<int> out_LineNumber;
    if(designEqualsImplementationClassSlot)
        out_LineNumber.reset(new int(-1));
    int statementOfSlotToGetLineNumberOf = 0;
    if(statementIndexOfSlot > -1)
        statementOfSlotToGetLineNumberOf = statementIndexOfSlot;
    if(!generateSourceCodePrivate(DesignEqualsImplementationProject::Library, cppEditTemporariesPath, true, out_LineNumber.data(), designEqualsImplementationClassSlot, statementOfSlotToGetLineNumberOf))
    {
        emit e("failed to generate temporary source code for c++ editting");
        return;
    }

    QProcess qtCreatorProcess(this); //TODOoptional: "-client" and "-block" args are of interest, as is "-pid" (and allowing user to specify it). "-client" and "-block" work best when there's already an instance of qt creator open, as then closing the document specified (or the project if no file was specified) via simple Ctrl+W makes the "blocking" continue.... which basically means: you keep qt creator and the project open, but can edit individual files still without having to re-open qtcreator or the project... FAST AS FUCK. however, if there is no qtcreator open when you do -client -block, one is opened for you and the -block does not continue until the entire qt creator is closed :(. So eh that "message box" explaining what is going to happen can/should advise you to open a qt creator beforehand (and maybe that should be the place to specify the optional PID of the instance to use (because shit, they might have 20 qt creators open)? (and btw, the pid should be remembered for the duration of this d=i session)). We advise them to open a qt creator before hitting "continue" -- a bit of refactoring needs to be done to generate c++ temporaries overwriting the old ones in order for the "fastness" to occur (and we should only ever overwrite files that have changed TODOoptimization... should make qtcreator's re-parsing much faster but i'm not sure about that)
    qtCreatorProcess.setWorkingDirectory(cppEditTemporariesPath);
    QStringList qtCreatorArgs;
    qtCreatorArgs << projectFileName();
    QString classSourceFilename;
    if(designEqualsImplementationClass)
        classSourceFilename = designEqualsImplementationClass->sourceFilenameOnly(); //TODOreq: if/when I introduce concept of sub-folders (and classes therein), I need to pass the source file's relative path here, not just the filename only
    if(designEqualsImplementationClassSlot)
    {
        if(*(out_LineNumber.data()) == -1)
        {
            emit e("didn't find line we were looking for. this should never happen");
            return;
        }
        classSourceFilename.append(":" + QString::number(*(out_LineNumber.data())));
    }
    if(designEqualsImplementationClass)
        qtCreatorArgs << classSourceFilename;
    qtCreatorProcess.start(existingQtCreator, qtCreatorArgs);
    if(!qtCreatorProcess.waitForStarted(-1))
    {
        emit e("qt creator failed to start: " + qtCreatorProcess.errorString());
        return;
    }
    if(!qtCreatorProcess.waitForFinished(-1))
    {
        emit e("qt creator failed to finish" + qtCreatorProcess.errorString());
        return;
    }
    if(qtCreatorProcess.exitCode() != 0 || qtCreatorProcess.exitStatus() != QProcess::NormalExit)
    {
        emit e("qt creator exitted abnormally" + qtCreatorProcess.errorString());
        return;
    }

    //qt creator session ended, so now parse/import in their raw c++

    Q_FOREACH(DesignEqualsImplementationClass *currentClass, classes())
    {
        QString sourceFilePath(cppEditTemporariesPath + currentClass->sourceFilenameOnly());
        QFile sourceFile(sourceFilePath);
        if(!sourceFile.open(QIODevice::ReadOnly))
        {
            emit e("failed to open for reading: " + sourceFilePath);
            return;
        }
        static const QString youFuckedUpBitch("you fucked with the delimiting comments, bro. don't. gj all your shit is fucked and is not being imported, but don't worry the files are still located at: " + cppEditTemporariesPath);
        QTextStream sourceFileTextStream(&sourceFile);
        bool inBeginChunkOfRawCppStatementsComment = false;
        QString className;
        int slotIndex = -1;
        int statementInsertIndex = -1;
        bool inChunkOfRawCppStatements = false;
        QStringList chunkOfRawCppStatements;
        int rollingStatementOffset = 0; //because the statement index changes as we insert them as we go
        QString lastSeenClass;
        int lastSeenSlotIndex = -1;
        while(!sourceFileTextStream.atEnd())
        {
            QString currentLine = sourceFileTextStream.readLine();
            QString currentLineTrimmed = currentLine.trimmed();
            if(currentLineTrimmed.isEmpty() || currentLineTrimmed == "//" || currentLineTrimmed == DEI_CHUNK_OF_RAW_CPP_GOES_HERE_HELPER_COMMENT)
                continue;
            if(!inChunkOfRawCppStatements)
            {
                if(!inBeginChunkOfRawCppStatementsComment)
                {
                    if(currentLine.contains("["  beginChunkOfRawCppStatements))
                    {
                        inBeginChunkOfRawCppStatementsComment = true;
                        continue;
                    }

                    //generated c++ we want to ignore
                    continue;
                }
                else
                {
                    static const QString classPrefix("Class: "); //TODOoptional: make this section, and the delimiting comments generating section, all refer to the same #defines
                    if(currentLine.contains(classPrefix))
                    {
                        QStringList classLineSplit = currentLine.split(classPrefix);
                        if(classLineSplit.size() != 2)
                        {
                            emit e(youFuckedUpBitch);
                            return;
                        }
                        className = classLineSplit.at(1);
                        if(className != lastSeenClass)
                        {
                            rollingStatementOffset = 0;
                        }
                        lastSeenClass = className;
                        continue;
                    }
                    static const QString slotIndexPrefix("SlotIndex: ");
                    if(currentLine.contains(slotIndexPrefix))
                    {
                        QStringList slotIndexLineSplit = currentLine.split(slotIndexPrefix);
                        if(slotIndexLineSplit.size() != 2)
                        {
                            emit e(youFuckedUpBitch);
                            return;
                        }
                        bool convertOk = false;
                        slotIndex = slotIndexLineSplit.at(1).toInt(&convertOk);
                        if(!convertOk)
                        {
                            emit e(youFuckedUpBitch);
                            return;
                        }
                        if(slotIndex != lastSeenSlotIndex)
                        {
                            rollingStatementOffset = 0;
                        }
                        lastSeenSlotIndex = slotIndex;
                        continue;
                    }
                    static const QString statementInsertIndexPrefix("StatementInsertIndex: ");
                    if(currentLine.contains(statementInsertIndexPrefix))
                    {
                        QStringList statementInsertIndexLineSplit = currentLine.split(statementInsertIndexPrefix);
                        if(statementInsertIndexLineSplit.size() != 2)
                        {
                            emit e(youFuckedUpBitch);
                            return;
                        }
                        bool convertOk = false;
                        statementInsertIndex = statementInsertIndexLineSplit.at(1).toInt(&convertOk);
                        if(!convertOk)
                        {
                            emit e(youFuckedUpBitch);
                            return;
                        }
                        continue;
                    }
                    if(currentLine.contains("[/" beginChunkOfRawCppStatements))
                    {
                        //sanitize
                        if(className.isEmpty() || slotIndex == -1 || statementInsertIndex == -1)
                        {
                            emit e(youFuckedUpBitch);
                            return;
                        }
                        inChunkOfRawCppStatements = true;
                        continue;
                    }

                    //should never get here
                    continue;
                }
            }
            else
            {
                if(currentLine.contains("[" endChunkOfRawCppStatements))
                {
                    //done gathering chunk of raw cpp statements, so put them into the list of statements, reset the values this while loop relies on, and then continue;

                    //make chunk of c++ statements a "statement" of the slot (so it gets generated etc)
                    if(!chunkOfRawCppStatements.isEmpty())
                    {
                        DesignEqualsImplementationClass *designEqualsImplementationClass = classFromClassName(className);
                        DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot = designEqualsImplementationClass->mySlots().at(slotIndex);
                        designEqualsImplementationClassSlot->insertStatementIntoOrderedListOfStatements(statementInsertIndex + (rollingStatementOffset), new DesignEqualsImplementationChunkOfRawCppStatements(chunkOfRawCppStatements));
                        ++rollingStatementOffset;
                    }

                    //reset everything to look for the next class/slot/statement in this source file
                    inBeginChunkOfRawCppStatementsComment = false;
                    inChunkOfRawCppStatements = false;
                    chunkOfRawCppStatements.clear();

                    continue;
                }

                //adding raw c++ statement to current chunk
                //chunkOfRawCppStatements << currentLine/*.trimmed()*/; //TO DOneoptional(nvm): maybe don't trim, since the c++ might contain curly brackets (scopes) that are indented
                //chunkOfRawCppStatements << currentLineTrimmed; //TODOreq: research something like: "qtcreator -auto-indent-files" -- maybe add it yourself
                chunkOfRawCppStatements << currentLine;
                continue;
            }
        }
    }
}
//TODOreq: overwrite warnings, backing up user modifications, putting "do not edit this file" warnings, etc etc
//TODOreq: enum to tell what kind of source to generate (cli, gui, lib, potentially 'web' eventually)
void DesignEqualsImplementationProject::generateSourceCode(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath)
{
    emit sourceCodeGenerated(generateSourceCodePrivate(projectGenerationMode, appendSlashIfNeeded(destinationDirectoryPath)));
}
#if 0
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion = DesignEqualsImplementationProject_SERIALIZATION_VERSION;
    DesignEqualsImplementationProject_QDS(<<, out, project);
}
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project)
{
    quint8 serializationVersion;
    DesignEqualsImplementationProject_QDS(>>, in, project);
}
#endif
