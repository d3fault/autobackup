#ifndef DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H
#define DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H

#include <QObject>
#include <QHash>

#include "designequalsimplementationproject.h"

class DesignEqualsImplementationClass;
class DesignEqualsImplementationUseCase;

class DesignEqualsImplementationProjectGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DesignEqualsImplementationProjectGenerator(DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode, const QString &destinationDirectoryPath, QObject *parent = 0);
    bool processClassStep0declaringClassInProject(DesignEqualsImplementationClass *designEqualsImplementationClass);
    bool processUseCase(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase);
    //bool processClassStep1writingTheFile(DesignEqualsImplementationClass *designEqualsImplementationClass);
    bool writeClassesToDisk();

    DesignEqualsImplementationProject::ProjectGenerationMode projectGenerationMode() const;
    void setProjectGenerationMode(const DesignEqualsImplementationProject::ProjectGenerationMode &projectGenerationMode);
    QString destinationDirectoryPath() const;
    void setDestinationDirectoryPath(const QString &destinationDirectoryPath);
private:
    DesignEqualsImplementationProject::ProjectGenerationMode m_ProjectGenerationMode;
    QString m_DestinationDirectoryPath;

    QHash<DesignEqualsImplementationClass*, QList<QString> /* connect statments in the key class's initialization sequence */> m_ClassesInThisProjectGenerate;

    //designEqualsImplementationUseCase, rootClassLifeline, designEqualsImplementationUseCase->m_UseCaseSlotEntryPointOnRootClassLifeline_OrFirstIsZeroIfNoneConnectedFromActorYet.second
    bool recursivelyWalkSlotInUseCaseModeAndAddAllAdditionalSLotsRelevantToThisUseCaseToQueueForGeneratingConnectStatements(DesignEqualsImplementationUseCase *designEqualsImplementationUseCase, DesignEqualsImplementationClassLifeLine *classLifeline, DesignEqualsImplementationClassSlot *slotToWalk);
    bool writeClassToDisk(DesignEqualsImplementationClass *currentClass);
    void appendConnectStatementToClassInitializationSequence(DesignEqualsImplementationClass *classToGetConnectStatementInInitializationSequence, const QString &connectStatement);
signals:
    void e(const QString &msg);
};

#endif // DESIGNEQUALSIMPLEMENTATIONPROJECTGENERATOR_H
