#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECT_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECT_H

#include <QObject>
#include <QList>

#include "designequalsimplementationcommon.h"
#include "designequalsimplementationclass.h" //TODOoptional: forward declare in this + tons of similar classes
#include "designequalsimplementationusecase.h"

class DesignEqualsImplementationProject : public QObject
{
    Q_OBJECT
public:
    enum ProjectGenerationMode { Library, CliApplication, GuiApplication /*, WebApplication */ };
    explicit DesignEqualsImplementationProject(QObject *parent = 0);
    ~DesignEqualsImplementationProject();

    DesignEqualsImplementationClass *createNewClass(const QString &newClassName = QString(), const QPointF &classPositionInGraphicsScene = QPointF());

    QList<QString> allKnownTypes() const;

    //TODOoptional: private + getter/setter blah
    QString Name;

    void addClass(DesignEqualsImplementationClass* newClass);
    QList<DesignEqualsImplementationClass*> classes();

    //DesignEqualsImplementationClassInstance *createTopLevelClassInstance(DesignEqualsImplementationClass* classToMakeTopLevelInstanceOf);
    //TODOinstancing: QList<DesignEqualsImplementationClassInstance *> topLevelClassInstances();

    void addUseCase(DesignEqualsImplementationUseCase *newUseCase);
    QList<DesignEqualsImplementationUseCase*> useCases();

    void noteDefinedElsewhereType(const QString &definedElsewhereType);
    QList<QString> definedElsewhereTypes() const;

    //TODOreq: m_Classes should be private, but I had issues getting the getters/setters to play nicely with QDataStream. Maybe a simple "friend QDataStream;" would fix it (or similar), but I can't be fucked to even play around with it right now. STILL, after coding for a while you should check that all usages of m_Classes are only from within the getter/setters (more important is the setter, but still in principle the getter too)
    QList<DesignEqualsImplementationClass*> m_Classes;
    QList<DesignEqualsImplementationUseCase*> m_UseCases;
    QList<QString> m_DefinedElsewhereTypes;

    //serialization and deserialization of a class REFERENCE
    inline int serializationClassIdForClass(DesignEqualsImplementationClass *classToReturnSerializationIdFor) { return m_Classes.indexOf(classToReturnSerializationIdFor); }
    inline DesignEqualsImplementationClass*classInstantiationFromSerializedClassId(int serializedClassId) { return m_Classes.at(serializedClassId); }

    //serializing and deserializing of a use case REFERENCE
    inline int serializationUseCaseIdForUseCase(DesignEqualsImplementationUseCase *useCase) { return m_UseCases.indexOf(useCase); }
    inline DesignEqualsImplementationUseCase* useCaseInstantiationFromSerializedUseCaseId(int useCaseId) { return m_UseCases.at(useCaseId); }

    //inline QString serializationTypeIdForType(const QString &type) { return type; } //TODOoptional: dictionary of types, change return type to int as optimization, etc

    QString projectFileName();

    inline DesignEqualsImplementationClass *classFromClassName(const QString &className)
    {
        Q_FOREACH(DesignEqualsImplementationClass *currentClass, m_Classes)
        {
            if(currentClass->ClassName == className)
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
    bool generateSourceCodePrivate(ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, bool generateCppEditModeDelimitingComments = false);
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
    void classAdded(DesignEqualsImplementationClass*);
    void sourceCodeGenerated(bool);
    void e(const QString &);
public slots:
    void emitAllClassesAndUseCasesInProject();
    void handleAddUmlItemRequested(UmlItemsTypedef umlItemType, QPointF position);
    void handleNewUseCaseRequested();
    void handleEditCppModeRequested();
    void generateSourceCode(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath);
};
#if 0
QDataStream &operator<<(QDataStream &out, const DesignEqualsImplementationProject &project);
QDataStream &operator>>(QDataStream &in, DesignEqualsImplementationProject &project);
#endif

Q_DECLARE_METATYPE(DesignEqualsImplementationProject*)

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECT_H
