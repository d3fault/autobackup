#include "designequalsimplementationprojectserializer.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"
#include "designequalsimplementationactor.h"
#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationprivatemethodsynchronouscallstatement.h"
#include "designequalsimplementationslotinvocationstatement.h"
#include "designequalsimplementationclasslifeline.h"

#define STREAM_OUT_METHOD_ARGUMENTS(qds, method) \
int numArgs = method->m_Arguments.size(); \
qds << numArgs; \
for(int i = 0; i < numArgs; ++i) \
{ \
    DesignEqualsImplementationClassMethodArgument *currentArgument = method->m_Arguments.at(i); \
    qds << currentArgument->Type; \
    qds << currentArgument->VariableName; \
}

#define STREAM_IN_METHOD_ARGUMENTS(qds, methodArguments) \
int numArgs; \
qds >> numArgs; \
for(int i = 0; i < numArgs; ++i) \
{ \
    QString argType; \
    QString argName; \
    qds >> argType; \
    qds >> argType; \
    methodArguments.append(qMakePair(argType, argName)); \
}

//not to be confused with project generation, this is saving/opening projects
//TODOreq: deserializing doesn't clear the fake temp slot, so it shows up in class diagram
//TODOreq: use case exit signal is not being [de-]serialized i'm pretty sure (but not 100% sure)
DesignEqualsImplementationProjectSerializer::DesignEqualsImplementationProjectSerializer(QObject *parent)
    : QObject(parent)
{ }
//save
void DesignEqualsImplementationProjectSerializer::serializeProjectToIoDevice(DesignEqualsImplementationProject *projectToSerialize, QIODevice *ioDeviceToSerializeTo)
{
    QDataStream projectDataStream(ioDeviceToSerializeTo); //TODOoptimization: qCompress, perhaps as an app setting (would mean we need a flag in beginning to tell us if compressed or not)
    //projectDataStream << *projectToSerialize;

    quint8 serializationVersion = 1;
    projectDataStream << serializationVersion;

    //Project
    projectDataStream << projectToSerialize->Name;

    projectDataStream << projectToSerialize->m_Classes.size();
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, projectToSerialize->m_Classes)
    {
        //Project Classes -- first declaration and most body serializing
        projectDataStream << currentClass->ClassName;
        projectDataStream << currentClass->Position;

        projectDataStream << currentClass->Properties.size();
        Q_FOREACH(DesignEqualsImplementationClassProperty *currentProperty, currentClass->Properties)
        {
            //Project Class Properties
            projectDataStream << *currentProperty; //POD so...
        }

        projectDataStream << currentClass->m_MySignals.size();
        Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, currentClass->m_MySignals)
        {
            //Project Class Signals
            projectDataStream << currentSignal->Name;
            STREAM_OUT_METHOD_ARGUMENTS(projectDataStream, currentSignal)
        }

        projectDataStream << currentClass->m_MySlots.size();
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, currentClass->m_MySlots)
        {
            //Project Class Slots
            projectDataStream << currentSlot->Name;
            STREAM_OUT_METHOD_ARGUMENTS(projectDataStream, currentSlot)
        }
    }
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, projectToSerialize->m_Classes)
    {
        //Project Classes -- second interdependent class serializing (hasAs are classes, so we had to hold off on setting up the hasAs in order to avoid dependency problems)
        projectDataStream << currentClass->m_HasA_Private_Classes_Members.size();
        Q_FOREACH(HasA_Private_Classes_Member *currentHasA, currentClass->m_HasA_Private_Classes_Members)
        {
            //Project Class HasA Classes Members
            projectDataStream << projectToSerialize->serializationClassIdForClass(currentHasA->m_MyClass);
            projectDataStream << currentHasA->VariableName;
        }
    }

    /*
    TODOreq: Slot Statements depend on class lifelines (and yes I was right, slot statements do [indirectly, through class lifelines] depend on m_HasA_Private_Classes_Members), class lifelines depend on use cases, use cases depend on statements ;-). However only the SignalSlotConnectionActivations portion of use cases depend on slot statements... So I need to do it in this order:
    1) UsesCase/ClassLifelines (no SignalSlotConnectionActivations)
    2) Slots Statements
    3) SignalSlotConnectionActivations in Use Cases
    */

    projectDataStream << projectToSerialize->m_UseCases.size();
    Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, projectToSerialize->m_UseCases)
    {
        //Project Use Cases (1/2) -- Everything that doesn't depend on slot statements //TODOreq: bug, lines/arrows aren't [de-]serialized (nor should they be, but they need to be redrawn based on SignalSlotConnectionActivations)
        projectDataStream << currentUseCase->Name;

        //Project Use Cases Actor
        bool actorInUseCase = (currentUseCase->m_UseCaseActor_OrZeroIfNoneAddedYet != 0);
        projectDataStream << actorInUseCase;
        if(actorInUseCase)
        {
            projectDataStream << currentUseCase->m_UseCaseActor_OrZeroIfNoneAddedYet->position();
        }

        projectDataStream << currentUseCase->m_ClassLifeLines.size();
        Q_FOREACH(DesignEqualsImplementationClassLifeLine *currentClassLifeLine, currentUseCase->m_ClassLifeLines)
        {
            //Project Use Case Class Lifelines
            projectDataStream << projectToSerialize->serializationClassIdForClass(currentClassLifeLine->m_DesignEqualsImplementationClass);
            projectDataStream << currentClassLifeLine->m_Position;
            projectDataStream << static_cast<quint8>(currentClassLifeLine->m_InstanceType);
            if(currentClassLifeLine->m_InstanceType == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline)
            {
                //out << *currentClassLifeLine->m_InstanceInOtherClassIfApplicable;
                DesignEqualsImplementationClass *parentClass = currentClassLifeLine->m_InstanceInOtherClassIfApplicable->m_ParentClass;
                projectDataStream << projectToSerialize->serializationClassIdForClass(parentClass);
                projectDataStream << parentClass->serializationHasAIdForHasA(currentClassLifeLine->m_InstanceInOtherClassIfApplicable);
            }

            //is 'known' because use case hasA class lifeline: out << classLifeline.m_ParentProject->serializationUseCaseIdForUseCase(classLifeline.parentUseCase());
            QList<SerializableSlotIdType> slotsAppearingInClassLifeline;
            Q_FOREACH(DesignEqualsImplementationClassSlot* currentSlot, currentClassLifeLine->m_MySlotsAppearingInClassLifeLine)
            {
                //Project Use Class Lifeline Slots
                slotsAppearingInClassLifeline.append(qMakePair(projectToSerialize->serializationClassIdForClass(currentSlot->ParentClass), currentSlot->ParentClass->serializationSlotIdForSlot(currentSlot)));
            }
            projectDataStream << slotsAppearingInClassLifeline;
        }

        //Project Use Cases Slot Entry Point
        DesignEqualsImplementationClassLifeLine *useCaseRootClassLifelineMaybe = currentUseCase->m_UseCaseSlotEntryPoint_OrFirstIsZeroIfNoneConnectedFromActorYet.first;
        bool useCaseHasUseCaseEntryPoint = (useCaseRootClassLifelineMaybe != 0);
        projectDataStream << useCaseHasUseCaseEntryPoint;
        if(useCaseHasUseCaseEntryPoint)
        {
            projectDataStream << currentUseCase->serializationClassLifelineIdForClassLifeline(useCaseRootClassLifelineMaybe);
            DesignEqualsImplementationClassSlot *slotEntryPoint = currentUseCase->m_UseCaseSlotEntryPoint_OrFirstIsZeroIfNoneConnectedFromActorYet.second;
            projectDataStream << slotEntryPoint->ParentClass->serializationSlotIdForSlot(slotEntryPoint);
        }
    }

    Q_FOREACH(DesignEqualsImplementationClass *currentClass, projectToSerialize->classes())
    {
        //Project Classes Slots Statements -- we had to hold off on populating the statements until all classes/signals/slots/private-methods, and even bare use cases/class-lifelines, were instantiated
        Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, currentClass->mySlots())
        {
            projectDataStream << currentSlot->m_OrderedListOfStatements.size();
            Q_FOREACH(IDesignEqualsImplementationStatement *currentStatement, currentSlot->m_OrderedListOfStatements)
            {
                //Project Class Slots Statements -- I don't think it's necessary since we iterate the slots the same way we create them, but we might need a slotId to know which slot these statements belong to
                projectDataStream << static_cast<quint8>(currentStatement->StatementType);
                currentStatement->streamOut(projectToSerialize, projectDataStream);
            }
        }
    }

    Q_FOREACH(DesignEqualsImplementationUseCase *currentUseCase, projectToSerialize->m_UseCases)
    {
        //Project Use Cases (2/2) -- Everything that depends on slot statements (such as SignalSlotConnectionActivations)

        projectDataStream << currentUseCase->m_SignalSlotConnectionActivationsInThisUseCase.size();
        Q_FOREACH(DesignEqualsImplementationUseCase::SignalSlotConnectionActivationTypeStruct currentSignalSlotActivation, currentUseCase->m_SignalSlotConnectionActivationsInThisUseCase)
        {
            //Project Use Case SignalSlotConnectionActivations

            //Signal
            projectDataStream << currentUseCase->serializationClassLifelineIdForClassLifeline(currentSignalSlotActivation.SignalStatement_Key0_SourceClassLifeLine);
            projectDataStream << qMakePair(projectToSerialize->serializationClassIdForClass(currentSignalSlotActivation.SignalStatement_Key1_SourceSlotItself->ParentClass), currentSignalSlotActivation.SignalStatement_Key1_SourceSlotItself->ParentClass->serializationSlotIdForSlot(currentSignalSlotActivation.SignalStatement_Key1_SourceSlotItself));
            projectDataStream << currentSignalSlotActivation.SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements;

            //Slots
            projectDataStream << currentSignalSlotActivation.SlotInvokedThroughConnection_Key0_IndexInto_m_ClassLifeLines;
            projectDataStream << qMakePair(projectToSerialize->serializationClassIdForClass(currentSignalSlotActivation.SlotInvokedThroughConnection_Key1_DestinationSlotItself->ParentClass), currentSignalSlotActivation.SlotInvokedThroughConnection_Key1_DestinationSlotItself->ParentClass->serializationSlotIdForSlot(currentSignalSlotActivation.SlotInvokedThroughConnection_Key1_DestinationSlotItself));
        }

        //Project Use Case Exit Signal -- TODOreq, and deserialize also ofc. but pending slot references in other use cases (not to be confused with SERIALIZED slot references)
    }
}
//open
void DesignEqualsImplementationProjectSerializer::deserializeProjectFromIoDevice(QIODevice *ioDeviceToDeserializeFrom, DesignEqualsImplementationProject *projectToPopulate)
{
    QDataStream projectDataStream(ioDeviceToDeserializeFrom);

    quint8 serializationVersion;
    projectDataStream >> serializationVersion;
    if(serializationVersion != 1)
        return;

    //Project
    projectDataStream >> projectToPopulate->Name;
    int numClasses;
    projectDataStream >> numClasses;
    for(int i = 0; i < numClasses; ++i)
    {
        //Project Classes -- first declaration and most body populating
        QString currentClassName;
        projectDataStream >> currentClassName;
        QPointF classPosition;
        projectDataStream >> classPosition;
        DesignEqualsImplementationClass *currentClass = projectToPopulate->createNewClass(currentClassName, classPosition);

        int numProperties;
        projectDataStream >>numProperties;
        for(int j = 0; j < numProperties; ++j)
        {
            //Project Class Properties
            DesignEqualsImplementationClassProperty *currentProperty = new DesignEqualsImplementationClassProperty(currentClass);
            projectDataStream >> *currentProperty; //POD so...
            currentClass->addProperty(currentProperty);
        }

        int numSignals;
        projectDataStream >> numSignals;
        for(int j = 0; j < numSignals; ++j)
        {
            //Project Class Signals
            QString signalName;
            projectDataStream >> signalName;
            QList<MethodArgumentTypedef> methodArguments;
            STREAM_IN_METHOD_ARGUMENTS(projectDataStream, methodArguments)
            currentClass->createNewSignal(signalName, methodArguments);
        }

        int numSlots;
        projectDataStream >> numSlots;
        for(int j = 0; j < numSlots; ++j)
        {
            //Project Class Slots
            QString slotName;
            projectDataStream >> slotName;
            QList<MethodArgumentTypedef> methodArguments;
            STREAM_IN_METHOD_ARGUMENTS(projectDataStream, methodArguments)
            currentClass->createwNewSlot(slotName, methodArguments);
        }
    }
    for(int i = 0; i < numClasses; ++i)
    {
        //Project Classes -- second interdependent class populating (hasAs are classes, so we had to hold off on setting up the hasAs in order to avoid dependency problems)
        int numHasAPrivateClassesMembers;
        projectDataStream >> numHasAPrivateClassesMembers;
        for(int j = 0; j < numHasAPrivateClassesMembers; ++j)
        {
            DesignEqualsImplementationClass *currentClass = projectToPopulate->m_Classes.at(i);
            //Project Class HasA Classes Members
            int hasAClassId;
            projectDataStream >> hasAClassId;
            QString hasAvariableName;
            projectDataStream >> hasAvariableName;
            currentClass->createHasA_Private_Classes_Member(projectToPopulate->classInstantiationFromSerializedClassId(hasAClassId), hasAvariableName);
        }
    }

    int numUseCases;
    projectDataStream >> numUseCases;
    for(int i = 0; i < numUseCases; ++i)
    {
        //Project Use Cases (1/2) -- Everything that doesn't depend on slot statements
        QString useCaseName;
        projectDataStream >> useCaseName;
        DesignEqualsImplementationUseCase *currentUseCase = new DesignEqualsImplementationUseCase(projectToPopulate, projectToPopulate);
        currentUseCase->Name = useCaseName;
        projectToPopulate->addUseCase(currentUseCase);

        //Project Use Cases Actor
        bool actorInUseCase;
        projectDataStream >> actorInUseCase;
        if(actorInUseCase)
        {
            QPointF actorPosition;
            projectDataStream >> actorPosition;
            currentUseCase->addActorToUseCase(actorPosition);
        }

        int numClassLifelines;
        projectDataStream >> numClassLifelines;
        for(int j = 0; j < numClassLifelines; ++j)
        {
            //Project Use Case Class Lifelines
            int classLifelineClassId;
            projectDataStream >> classLifelineClassId;
            DesignEqualsImplementationClass *classLifelineClass = projectToPopulate->classInstantiationFromSerializedClassId(classLifelineClassId);
            QPointF classLifelinePosition;
            projectDataStream >> classLifelinePosition;
            DesignEqualsImplementationClassLifeLine *currentClassLifeline = currentUseCase->createClassLifelineInUseCase(classLifelineClass, classLifelinePosition, false);
            quint8 classLifelineInstanteType;
            projectDataStream >> classLifelineInstanteType;
            currentClassLifeline->m_InstanceType = static_cast<DesignEqualsImplementationClassLifeLine::DesignEqualsImplementationClassInstanceTypeEnum>(classLifelineInstanteType);
            if(currentClassLifeline->m_InstanceType == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline)
            {
                int classIdofParentThatHasAus; //class id of parent that has us as a member
                projectDataStream >> classIdofParentThatHasAus;
                DesignEqualsImplementationClass *hasAparentClass = projectToPopulate->classInstantiationFromSerializedClassId(classIdofParentThatHasAus);
                int hasAid;
                projectDataStream >> hasAid;
                HasA_Private_Classes_Member *instanceInOtherClass = hasAparentClass->hasAinstanceFromHasAId(hasAid);
                currentClassLifeline->setInstanceInOtherClassIfApplicable(instanceInOtherClass);
            }
            currentClassLifeline->m_ParentUseCase = currentUseCase;

            QList<SerializableSlotIdType> slotsAppearingInClassLifeline;
            projectDataStream >> slotsAppearingInClassLifeline;
            Q_FOREACH(SerializableSlotIdType currentSlotReference, slotsAppearingInClassLifeline)
            {
                //Project Use Class Lifeline Slots
                currentClassLifeline->m_MySlotsAppearingInClassLifeLine.append(projectToPopulate->classInstantiationFromSerializedClassId(currentSlotReference.first)->slotInstantiationFromSerializedSlotId(currentSlotReference.second));
            }
        }

        //Project Use Cases Slot Entry Point
        bool useCaseHasUseCaseEntryPoint;
        projectDataStream >> useCaseHasUseCaseEntryPoint;
        if(useCaseHasUseCaseEntryPoint)
        {
            int rootClassLifelineId;
            projectDataStream >> rootClassLifelineId;
            int slotEntryPointSlotId;
            projectDataStream >> slotEntryPointSlotId;
            DesignEqualsImplementationClassLifeLine *rootClassLifeline = currentUseCase->classLifelineInstantiatedFromSerializedClassLifelineId(rootClassLifelineId);
            currentUseCase->setUseCaseSlotEntryPoint(rootClassLifeline, rootClassLifeline->designEqualsImplementationClass()->slotInstantiationFromSerializedSlotId(slotEntryPointSlotId));
        }
    }


    Q_FOREACH(DesignEqualsImplementationClass *currentClass, projectToPopulate->classes())
    {
        //Project Classes Slots Statements -- we had to hold off on populating the statements until all classes/signals/slots/private-methods, and even bare use cases/class lifelines, were instantiated
        int numSlots = currentClass->mySlots().size();
        //segfaults: Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, currentClass->mySlots())
        for(int i = 0; i < numSlots; ++i)
        {
            DesignEqualsImplementationClassSlot *currentSlot = currentClass->mySlots().at(i); //TODOreq(semi-fixed through bool flag in class lifeline constructor, but seems hacky): if the temp slot hack was made when class lifeline was constructed (methinks it was), then 'i' won't point to the correct slot! fml
            int numOrderedListOfStatements;
            projectDataStream >> numOrderedListOfStatements;
            for(int i = 0; i < numOrderedListOfStatements; ++i)
            {
                //Project Class Slots Statements
                quint8 currentStatementType;
                projectDataStream >> currentStatementType;
                IDesignEqualsImplementationStatement *statement;
                switch(static_cast<IDesignEqualsImplementationStatement::StatementTypeEnum>(currentStatementType))
                {
                case IDesignEqualsImplementationStatement::SignalEmitStatementType:
                    statement = new DesignEqualsImplementationSignalEmissionStatement();
                    break;
                case IDesignEqualsImplementationStatement::SlotInvokeStatementType: //TODOreq: [de-]serialize class lifeline containing slot to invoke
                    statement = new DesignEqualsImplementationSlotInvocationStatement();
                    break;
                case IDesignEqualsImplementationStatement::PrivateMethodSynchronousCallStatementType:
                    statement = new DesignEqualsImplementationPrivateMethodSynchronousCallStatement();
                    break;
                }
                statement->streamIn(projectToPopulate, projectDataStream); //TO DOnereq: i am pretty sure this needs to happen on another/later iteration of the slots, because not all slots/signals/etc have been instantiated (and hell, not even all the classes have been instantiated either!). similar to the 'second interdependent class populating' below. dependency problems sums it up perfectly. i probably DON'T need to move the stream out code, but i should keep them nice and lined up so that's reason enough to yes move it. it should probably come after the [similar] m_HasA_Private_Classes_Members second iteration, becuase these statements themselves MIGHT depend on those as well! not sure though. the statement populating must come after SIGNAL/SLOT/PRIVATE-METHOD (and any other 'statement type')
                currentSlot->m_OrderedListOfStatements.append(statement);
            }
        }
    }

    for(int i = 0; i < numUseCases; ++i)
    {
        //Project Use Cases (2/2) -- Everything that depends on slot statements (such as SignalSlotConnectionActivations)

        DesignEqualsImplementationUseCase *currentUseCase = projectToPopulate->useCases().at(i);

        int numSignalSlotConnectionActivationsInThisUseCase;
        projectDataStream >> numSignalSlotConnectionActivationsInThisUseCase;
        for(int j = 0; j < numSignalSlotConnectionActivationsInThisUseCase; ++j)
        {
            //Project Use Case SignalSlotConnectionActivations            
            DesignEqualsImplementationUseCase::SignalSlotConnectionActivationTypeStruct signalSlotConnectionActivation;
            //Signal
            //SignalStatement_Key0_IndexInto_m_ClassLifeLines
            int signalStatement_Key0_IndexInto_m_ClassLifeLines;
            projectDataStream >> signalStatement_Key0_IndexInto_m_ClassLifeLines;
            signalSlotConnectionActivation.SignalStatement_Key0_SourceClassLifeLine = currentUseCase->classLifelineInstantiatedFromSerializedClassLifelineId(signalStatement_Key0_IndexInto_m_ClassLifeLines);

            //SignalStatement_Key1_SourceSlotItself
            SerializableSlotIdType serializedSourceSlotId;
            projectDataStream >> serializedSourceSlotId;
            signalSlotConnectionActivation.SignalStatement_Key1_SourceSlotItself = currentUseCase->m_DesignEqualsImplementationProject->classInstantiationFromSerializedClassId(serializedSourceSlotId.first)->slotInstantiationFromSerializedSlotId(serializedSourceSlotId.second);

            //SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements
            projectDataStream >> signalSlotConnectionActivation.SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements;


            //Slots
            //SlotInvokedThroughConnection_Key0_IndexInto_m_ClassLifeLines
            projectDataStream >> signalSlotConnectionActivation.SlotInvokedThroughConnection_Key0_IndexInto_m_ClassLifeLines;
            //SlotInvokedThroughConnection_Key1_DestinationSlotItself
            SerializableSlotIdType serializedDestinationedSlotId;
            projectDataStream >> serializedDestinationedSlotId;
            signalSlotConnectionActivation.SlotInvokedThroughConnection_Key1_DestinationSlotItself = currentUseCase->m_DesignEqualsImplementationProject->classInstantiationFromSerializedClassId(serializedDestinationedSlotId.first)->slotInstantiationFromSerializedSlotId(serializedDestinationedSlotId.second); //TODOoptimization: class id is not needed, because class lifeline was just streamed, so can be determined indirectly

            currentUseCase->m_SignalSlotConnectionActivationsInThisUseCase.append(signalSlotConnectionActivation);
        }
    }
}
