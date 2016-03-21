#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H

#include <QObject>
#include <QHash>
#include <QTextStream>

#include "designequalsimplementationproject.h"

#define chunkOfRawCppStatementsSuffix " CHUNK OF RAW CPP STATEMENTS]"
#define beginChunkOfRawCppStatements "BEGIN" chunkOfRawCppStatementsSuffix
#define endChunkOfRawCppStatements "END" chunkOfRawCppStatementsSuffix
#define DEI_CHUNK_OF_RAW_CPP_GOES_HERE_HELPER_COMMENT "//====CHUNK OF RAW CPP GOES HERE===="

class DesignEqualsImplementationClass;
class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationProjectGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectGenerator(DesignEqualsImplementationProject *designEqualsImplementationProject, DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, bool generateCppEditModeDelimitingComments = false, int *m_Out_LineNumberToJumpTo_OrZeroIfNotApplicable = 0, DesignEqualsImplementationClassSlot *m_SlotWeWantLineNumberOf_OnlyWhenApplicable = 0, int m_StatementIndexOfSlotToGetLineNumberOf_OnlyWhenApplicable = -1, QObject *parent = 0);
    bool generateProjectFileAndWriteItToDisk();
    bool processClassStep0declaringClassInProject(DesignEqualsImplementationClass *designEqualsImplementationClass);
    bool processUseCase(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase);
    //bool processClassStep1writingTheFile(DesignEqualsImplementationClass *designEqualsImplementationClass);
    bool writeClassesToDisk();

    DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode() const;
    void setProjectGenerationMode(const DesignEqualsImplementationProject::ProjectGenerationMode &projectGenerationMode);
    QString destinationDirectoryPath() const;
    void setDestinationDirectoryPath(const QString &destinationDirectoryPath);
    static inline QString firstCharacterToUpper(const QString &inputString)
    {
        QString ret = inputString;
        if(!inputString.isEmpty())
        {
            QString firstChar = ret.at(0);
            QString firstCharToUpper = firstChar.toUpper();
            ret.replace(0, 1, firstCharToUpper);
        }
        return ret;
    }
    static inline QString memberNameForProperty(const QString &thePropertyName)
    {
        QString ret = "m_" + firstCharacterToUpper(thePropertyName);
        return ret;
    }
    static inline QString firstCharacterToLower(const QString &inputString)
    {
        QString ret = inputString;
        if(!inputString.isEmpty())
        {
            QString firstChar = ret.at(0);
            QString firstCharToLower = firstChar.toLower();
            ret.replace(0, 1, firstCharToLower);
        }
        return ret;
    }
    static inline QString getterNameForProperty(const QString &thePropertyName)
    {
        QString ret = "get" + firstCharacterToUpper(thePropertyName);
        return ret;
    }
    static inline QString setterNameForProperty(const QString &thePropertyName)
    {
        QString ret = "set" + firstCharacterToUpper(thePropertyName);
        return ret;
    }
    static inline QString changedSignalForProperty(const QString &thePropertyName)
    {
        QString ret = firstCharacterToLower(thePropertyName) + "Changed";
        return ret;
    }
private:
    DesignEqualsImplementationProject *m_DesignEqualsImplementationProject;
    DesignEqualsImplementationProject::ProjectGenerationMode m_ProjectGenerationMode;
    QString m_DestinationDirectoryPath;

    bool m_GenerateCppEditModeDelimitingComments;
    int *m_Out_LineNumberToJumpTo_OrZeroIfNotApplicable;
    DesignEqualsImplementationClassSlot *m_SlotWeWantLineNumberOf_OnlyWhenApplicable;
    int m_StatementIndexOfSlotToGetLineNumberOf_OnlyWhenApplicable;
    QByteArray m_NewlinesCounterHackInMemoryCopyOfSourceFileByteArray;
    QTextStream m_NewlinesCounterHackInMemoryCopyOfSourceFileTextStream;

    QHash<DesignEqualsImplementationClass*, QList<QString> /* connect statments in the key class's initialization sequence */> m_ClassesInThisProjectGenerate_AndTheirCorrespondingConstructorConnectStatements;

    //designEqualsImplementationUseCase, rootClassLifeline, designEqualsImplementationUseCase->m_UseCaseSlotEntryPointOnRootClassLifeline_OrFirstIsZeroIfNoneConnectedFromActorYet.second
    bool recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSlotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, DesignEqualsImplementationClassLifeLine *classLifeline, DesignEqualsImplementationClassSlot *slotToWalk);
    bool writeClassToDisk(DesignEqualsImplementationClass *currentClass);
    void appendConnectStatementToClassInitializationSequence(DesignEqualsImplementationClass *classToGetConnectStatementInInitializationSequence, const QString &connectStatement);
    void writePairOfDelimitedCommentsInBetweenWhichAchunkOfRawCppStatementsCanBeWritten(QTextStream &sourceFileTextStream, const QString &className, int slotIndex, int statementInsertIndex);
    int numNewlinesInSourceFileAtThisPoint();
signals:
    void e(const QString &msg);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H
