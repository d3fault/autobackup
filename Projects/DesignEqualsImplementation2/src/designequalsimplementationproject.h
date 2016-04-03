#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECT_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECT_H

#include <QObject>
#include <QList>

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationclass.h" //TODOoptional: forward declare in this + tons of similar classes
#include "designequalsimplementationusecase.h"
#include "designequalsimplementationtype.h"

class DesignEqualsImplementationImplicitlySharedDataType;

class DesignEqualsImplementationProject : public QObject
{
    Q_OBJECT
public:
    enum ProjectGenerationMode { Library, CliApplication, GuiApplication /*, WebApplication */ };
    explicit DesignEqualsImplementationProject(QObject *parent = 0);
    ~DesignEqualsImplementationProject();

    DesignEqualsImplementationClass *createNewClass(const QString &newClassName = QString(), const QPointF &classPositionInGraphicsScene = QPointF());
    DesignEqualsImplementationImplicitlySharedDataType *createNewImplicitlySharedDataType(const QString &newImplicitlySharedDataTypeName = QString(), const QPointF &positionInGraphicsScene = QPointF());

    void addType(DesignEqualsImplementationType *type);
    QList<DesignEqualsImplementationType *> allKnownTypes() const;
    QList<QString> allKnownTypesNames() const;
    QList<QString> allKnownTypesNamesExcludingBuiltIns() const;
    DesignEqualsImplementationType *getOrCreateTypeFromName(const QString &nonQualifiedTypeName);

    //TODOoptional: private + getter/setter blah
    QString Name;

    QList<DesignEqualsImplementationClass*> classes();

    //DesignEqualsImplementationClassInstance *createTopLevelClassInstance(DesignEqualsImplementationClass* classToMakeTopLevelInstanceOf);
    //TODOinstancing: QList<DesignEqualsImplementationClassInstance *> topLevelClassInstances();

    void addUseCase(DesignEqualsImplementationUseCase *newUseCase);
    QList<DesignEqualsImplementationUseCase*> useCases();

    DefinedElsewhereType *noteDefinedElsewhereType(const QString &definedElsewhereType);
    //QList<DefinedElsewhereType*> definedElsewhereTypes() const;

    void ensureParsedBuiltInTypeHasType(const QString &parsedUnqualifiedType, ParsedTypeInstance::ParsedTypeInstanceCategoryEnum parsedTypeCategory);
    void ensureParsedBuiltInTypesHaveTypes(const QList<ParsedTypeInstance> &parsedTypes);
    void noteBuiltInType(const QString &builtInType);

    //TODOreq: m_Classes should be private, but I had issues getting the getters/setters to play nicely with QDataStream. Maybe a simple "friend QDataStream;" would fix it (or similar), but I can't be fucked to even play around with it right now. STILL, after coding for a while you should check that all usages of m_Classes are only from within the getter/setters (more important is the setter, but still in principle the getter too)

    QList<DesignEqualsImplementationUseCase*> m_UseCases;

    //serialization and deserialization of a type REFERENCE
    int serializationTypeIdForType(DesignEqualsImplementationType *typeToReturnSerializationIdFor);
    DesignEqualsImplementationType *typeFromSerializedTypeId(int serializedTypeId);

    //serializing and deserializing of a use case REFERENCE
    inline int serializationUseCaseIdForUseCase(DesignEqualsImplementationUseCase *useCase) { return m_UseCases.indexOf(useCase); }
    inline DesignEqualsImplementationUseCase* useCaseInstantiationFromSerializedUseCaseId(int useCaseId) { return m_UseCases.at(useCaseId); }

    //inline QString serializationTypeIdForType(const QString &type) { return type; } //TODOoptional: dictionary of types, change return type to int as optimization, etc

    QString projectFileName();

    inline DesignEqualsImplementationClass *classFromClassName(const QString &className)
    {
        Q_FOREACH(DesignEqualsImplementationClass *currentClass, classes())
        {
            if(currentClass->Name == className)
            {
                return currentClass;
            }
        }
        emit e("classFromClassName could not find class: " + className);
        return 0;
    }

    //Temporary for code gen:
    bool writeTemporaryGlueCodeLines(const QString &destinationDirectoryPath);
private:
    QList<DesignEqualsImplementationType*> m_AllKnownTypes;

    bool generateSourceCodePrivate(ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, bool generateCppEditModeDelimitingComments = false, int *out_LineNumberToJumpTo_OrZeroIfNotApplicable = 0, DesignEqualsImplementationClassSlot *slotWeWantLineNumberOf_OnlyWhenApplicable = 0, int statementIndexOfSlotToGetLineNumberOf_OnlyWhenApplicable = -1);
    bool tempGenerateHardcodedUiFiles(const QString &destinationDirectoryPath);
    bool allClassLifelinesInAllUseCasesInProjectHaveBeenAssignedInstances();
    inline QString appendSlashIfNeeded(const QString &inputString) { return inputString.endsWith("/") ? inputString : (inputString + "/"); }

#if 0
    //Temporary for code gen:
    void cleanupJitGeneratedLinesFromAPreviousGenerate();
    QList<QString> m_TemporaryProjectGlueCodeLines;
#endif
signals:
    void useCaseAdded(DesignEqualsImplementationUseCase*);
    void typeAdded(DesignEqualsImplementationType*);
    void sourceCodeGenerated(bool);
    void e(const QString &);
public slots:
    void emitAllClassesAndUseCasesInProject();
    void handleAddUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position);
    void handleNewUseCaseRequested();
    void handleEditCppModeRequested(DesignEqualsImplementationType *designEqualsImplementationClass = 0, DesignEqualsImplementationClassSlot *designEqualsImplementationClassSlot = 0, int statementIndexOfSlot = -1);
    void generateSourceCode(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath);
};
#if 0
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project);
#endif

Q_DECLARE_METATYPE(DesignEqualsImplementationProject*)

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECT_H
