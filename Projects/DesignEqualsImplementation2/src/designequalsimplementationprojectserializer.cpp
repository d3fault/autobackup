#include "designequalsimplementationprojectserializer.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"
#include "designequalsimplementationactor.h"
#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationprivatemethodsynchronouscallstatement.h"
#include "designequalsimplementationslotinvocationstatement.h"
#include "designequalsimplementationchunkofrawcppstatements.h"
#include "designequalsimplementationclasslifeline.h"
#include "designequalsimplementationimplicitlyshareddatatype.h"

#define STREAM_OUT_METHOD_ARGUMENTS(qds, method, project) \
int numArgs = method->arguments().size(); \
qds << numArgs; \
for(int i = 0; i < numArgs; ++i) \
{ \
    DesignEqualsImplementationClassMethodArgument *currentArgument = method->arguments().at(i); \
    qds << project->serializationTypeIdForType(currentArgument->type); \
    qds << currentArgument->VariableName; \
}

#define STREAM_IN_METHOD_ARGUMENTS(qds, method, project) \
int numArgs; \
qds >> numArgs; \
for(int i = 0; i < numArgs; ++i) \
{ \
    int argTypeId; \
    qds >> argTypeId; \
    Type *argType = project->typeFromSerializedTypeId(argTypeId); \
    QString argName; \
    qds >> argName; \
    method->createNewArgument(argType, argName); \
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

    projectDataStream << projectToSerialize->allKnownTypes().size();
    //First just serialize the type type (d=iClass, ImplicitlyShared, or DefinedElsewhere -- lolwut) and name
    int numTypesWithGreaterThanZeroDirectAncestors = 0;
    Q_FOREACH(Type *currentType, projectToSerialize->allKnownTypes())
    {
        projectDataStream << currentType->typeCategory();
        projectDataStream << currentType->Name;
        if(!currentType->DirectAncestors.isEmpty())
            ++numTypesWithGreaterThanZeroDirectAncestors; //just counting atm
        //currentType->serializeDescendentDetails(projectDataStream); //TODOreq: include descendent type id i think?
    }
    //Now serialize the type ANCESTORS (which, themselves, are/depend-on types)
    projectDataStream << numTypesWithGreaterThanZeroDirectAncestors;
    Q_FOREACH(Type *currentType, projectToSerialize->allKnownTypes())
    {
        if(!currentType->DirectAncestors.isEmpty())
        {
            projectDataStream << projectToSerialize->serializationTypeIdForType(currentType);
            projectDataStream << currentType->DirectAncestors.size();
            Q_FOREACH(const TypeAncestor &currentAncestor, currentType->DirectAncestors)
            {
                projectDataStream << static_cast<quint8>(currentAncestor.visibility);
                projectDataStream << projectToSerialize->serializationTypeIdForType(currentAncestor.type);
            }
        }
    }
    //now do typeCategory specific serializing
    Q_FOREACH(Type *currentType, projectToSerialize->allKnownTypes())
    {
        if(DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(currentType))
        {
            //Project Classes -- Position/signals/slots, but not NonFunctionMembers yet
            projectDataStream << typeAsClass->Position;

            projectDataStream << typeAsClass->m_MySignals.size();
            Q_FOREACH(DesignEqualsImplementationClassSignal *currentSignal, typeAsClass->m_MySignals)
            {
                //Project Class Signals
                projectDataStream << currentSignal->Name;
                STREAM_OUT_METHOD_ARGUMENTS(projectDataStream, currentSignal, projectToSerialize)
            }

            projectDataStream << typeAsClass->m_MySlots.size();
            Q_FOREACH(DesignEqualsImplementationClassSlot *currentSlot, typeAsClass->m_MySlots)
            {
                //Project Class Slots
                projectDataStream << currentSlot->Name;
                STREAM_OUT_METHOD_ARGUMENTS(projectDataStream, currentSlot, projectToSerialize)
            }
        }
        //else if: nothing to do for ImplicitlyShared or DefinedElsewhere atm
    }
    //Type NonFunctionMembers
    Q_FOREACH(Type *currentType, projectToSerialize->allKnownTypes())
    {
        if(qobject_cast<DefinedElsewhereType*>(currentType))
            continue; //can't have members (that we defined)

        projectDataStream << currentType->nonFunctionMembers().size();
        Q_FOREACH(NonFunctionMember *currentNonFunctionMember, currentType->nonFunctionMembers())
        {
            //Project Class NonFunctionMembers
            projectDataStream << projectToSerialize->serializationTypeIdForType(currentNonFunctionMember->type);
            projectDataStream << currentNonFunctionMember->VariableName;
            //projectDataStream << projectToSerialize->serializationTypeIdForType(currentNonFunctionMember->parentClass());
            projectDataStream << currentNonFunctionMember->HasInit;
            projectDataStream << currentNonFunctionMember->OptionalInit;
            projectDataStream << static_cast<quint8>(currentNonFunctionMember->visibility);
            projectDataStream << static_cast<quint8>(currentNonFunctionMember->OwnershipOfPointedTodataIfPointer);

            if(!qobject_cast<DesignEqualsImplementationClass*>(currentType))
                continue; //only "Class"es can have Q_PROPERTY
            bool isProperty = false;
            DesignEqualsImplementationClassProperty *property = qobject_cast<DesignEqualsImplementationClassProperty*>(currentNonFunctionMember);
            if(property)
                isProperty = true;
            projectDataStream << isProperty;
            if(isProperty)
            {
                projectDataStream << property->ReadOnly;
                projectDataStream << property->NotifiesOnChange;
            }
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
            projectDataStream << projectToSerialize->serializationTypeIdForType(currentClassLifeLine->m_DesignEqualsImplementationClass);
            projectDataStream << currentClassLifeLine->m_Position;
            projectDataStream << static_cast<quint8>(currentClassLifeLine->m_InstanceType);
            if(currentClassLifeLine->m_InstanceType == DesignEqualsImplementationClassLifeLine::ChildMemberOfOtherClassLifeline)
            {
                //out << *currentClassLifeLine->m_InstanceInOtherClassIfApplicable;
                Type *parentClass = currentClassLifeLine->m_InstanceInOtherClassIfApplicable->parentClass();
                projectDataStream << projectToSerialize->serializationTypeIdForType(parentClass);
                projectDataStream << parentClass->serializationNonFunctionMemberIdForNonFunctionMember(currentClassLifeLine->m_InstanceInOtherClassIfApplicable);
            }

            //is 'known' because use case hasA class lifeline: out << classLifeline.m_ParentProject->serializationUseCaseIdForUseCase(classLifeline.parentUseCase());
            QList<SerializableSlotIdType> slotsAppearingInClassLifeline;
            Q_FOREACH(DesignEqualsImplementationClassSlot* currentSlot, currentClassLifeLine->m_MySlotsAppearingInClassLifeLine)
            {
                //Project Use Class Lifeline Slots
                slotsAppearingInClassLifeline.append(qMakePair(projectToSerialize->serializationTypeIdForType(currentSlot->ParentClass), currentSlot->ParentClass->serializationSlotIdForSlot(currentSlot)));
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
            Q_FOREACH(IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *currentStatement, currentSlot->m_OrderedListOfStatements)
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
        Q_FOREACH(DesignEqualsImplementationUseCase::SignalSlotConnectionActivationTypeStruct *currentSignalSlotActivation, currentUseCase->m_SignalSlotConnectionActivationsInThisUseCase)
        {
            //Project Use Case SignalSlotConnectionActivations

            //Signal
            projectDataStream << currentUseCase->serializationClassLifelineIdForClassLifeline(currentSignalSlotActivation->SignalStatement_Key0_SourceClassLifeLine);
            projectDataStream << qMakePair(projectToSerialize->serializationTypeIdForType(currentSignalSlotActivation->SignalStatement_Key1_SourceSlotItself->ParentClass), currentSignalSlotActivation->SignalStatement_Key1_SourceSlotItself->ParentClass->serializationSlotIdForSlot(currentSignalSlotActivation->SignalStatement_Key1_SourceSlotItself));
            projectDataStream << currentSignalSlotActivation->SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements;

            //Slots
            projectDataStream << currentSignalSlotActivation->SlotsAttachedToTheSignal.size();
            Q_FOREACH(SlotConnectedToSignalTypedef *currentSlotAttachedToSignal, currentSignalSlotActivation->SlotsAttachedToTheSignal)
            {
                projectDataStream << currentSlotAttachedToSignal->first;
                projectDataStream << qMakePair(projectToSerialize->serializationTypeIdForType(currentSlotAttachedToSignal->second->ParentClass), currentSlotAttachedToSignal->second->ParentClass->serializationSlotIdForSlot(currentSlotAttachedToSignal->second));
            }
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

    int numTypes;
    projectDataStream >> numTypes;
    for(int i = 0; i < numTypes; ++i)
    {
        int typeCategory;
        projectDataStream >> typeCategory;
        Type *type;
        switch(typeCategory)
        {
        case 0:
            type = new DesignEqualsImplementationClass(projectToPopulate, projectToPopulate);
            break;
        case 1:
            type = new DesignEqualsImplementationImplicitlySharedDataType(projectToPopulate);
            break;
        case 2:
            type = new DefinedElsewhereType(projectToPopulate);
        default: //ODOoptional: use enum so I don't need default case so adding types would give compiler error here (good!)
            qFatal("invalid typeType! your guess is as good as mine");
            break;
        }
        projectDataStream >> type->Name;
        projectToPopulate->addType(type);
    }
    int numTypesWithAncestors;
    projectDataStream >> numTypesWithAncestors;
    for(int i = 0; i < numTypesWithAncestors; ++i)
    {
        int typeWithAncestorsTypeId;
        projectDataStream >> typeWithAncestorsTypeId;
        Type *typeWithAncestor = projectToPopulate->typeFromSerializedTypeId(typeWithAncestorsTypeId);
        int numAncestors;
        projectDataStream >> numAncestors;
        for(int j = 0; j < numAncestors; ++j)
        {
            TypeAncestor ancestor;
            quint8 visibility;
            projectDataStream >> visibility;
            ancestor.visibility = static_cast<Visibility::VisibilityEnum>(visibility);
            int ancestorTypeId;
            projectDataStream >> ancestorTypeId;
            ancestor.type = projectToPopulate->typeFromSerializedTypeId(ancestorTypeId);
            typeWithAncestor->DirectAncestors.append(ancestor);
        }
    }
    //now do typeCategory specific deserializing
    Q_FOREACH(Type *currentType, projectToPopulate->allKnownTypes())
    {
        if(DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(currentType))
        {
            //Project Classes -- Position/signals/slots, but not NonFunctionMembers yet
            projectDataStream >> typeAsClass->Position;

            int numSignals;
            projectDataStream >> numSignals;
            for(int j = 0; j < numSignals; ++j)
            {
                //Project Class Signals
                QString signalName;
                projectDataStream >> signalName;
                DesignEqualsImplementationClassSignal *theSignal = typeAsClass->createNewSignal(signalName);
                STREAM_IN_METHOD_ARGUMENTS(projectDataStream, theSignal, projectToPopulate)
            }

            int numSlots;
            projectDataStream >> numSlots;
            for(int j = 0; j < numSlots; ++j)
            {
                //Project Class Slots
                QString slotName;
                projectDataStream >> slotName;
                DesignEqualsImplementationClassSlot *theSlot = typeAsClass->createwNewSlot(slotName);
                STREAM_IN_METHOD_ARGUMENTS(projectDataStream, theSlot, projectToPopulate)
            }
        }
        //else if: nothing to do for ImplicitlyShared or DefinedElsewhere atm
    }
    //Type NonFunctionMembers
    Q_FOREACH(Type *currentType, projectToPopulate->allKnownTypes())
    {
        if(qobject_cast<DefinedElsewhereType*>(currentType))
            continue; //can't have members (that we defined)
        int numNonFunctionMembers;
        projectDataStream >> numNonFunctionMembers;
        for(int j = 0; j < numNonFunctionMembers; ++j)
        {
            int nonFunctionMemberTypeId;
            projectDataStream >> nonFunctionMemberTypeId;
            QString nonFunctionMemberVariableName;
            projectDataStream >> nonFunctionMemberVariableName;
            //int nonFunctionMemberParentId;
            //projectDataStream >> nonFunctionMemberParentId;
            bool hasInit;
            projectDataStream >> hasInit;
            QString optionalInit;
            projectDataStream >> optionalInit;
            quint8 visibility;
            projectDataStream >> visibility;
            quint8 ownershipOfPointedToDataIfpointer;
            projectDataStream >> ownershipOfPointedToDataIfpointer;

            if(DesignEqualsImplementationClass *typeAsClass = qobject_cast<DesignEqualsImplementationClass*>(currentType))
            {
                //only "Class"es can have Q_PROPERTY
                bool isProperty;
                projectDataStream >> isProperty;
                if(isProperty)
                {
                    bool readOnly;
                    projectDataStream >> readOnly;
                    bool notifiesOnChange;
                    projectDataStream >> notifiesOnChange;

                    typeAsClass->createNewProperty(projectToPopulate->typeFromSerializedTypeId(nonFunctionMemberTypeId), nonFunctionMemberVariableName, hasInit, optionalInit, readOnly, notifiesOnChange);
                    continue;
                }
            }

            //not a "Class" nor a Q_PROPERTY
            currentType->createNewNonFunctionMember(projectToPopulate->typeFromSerializedTypeId(nonFunctionMemberTypeId), nonFunctionMemberVariableName, static_cast<Visibility::VisibilityEnum>(visibility), static_cast<TypeInstanceOwnershipOfPointedToDataIfPointer::TypeInstanceOwnershipOfPointedToDataIfPointerEnum>(ownershipOfPointedToDataIfpointer), hasInit, optionalInit);
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
            DesignEqualsImplementationClass *classLifelineClass = qobject_cast<DesignEqualsImplementationClass*>(projectToPopulate->typeFromSerializedTypeId(classLifelineClassId));
            if(!classLifelineClass)
                qFatal("While trying to deserialize a class lifeline, the type for which the class lifeline allegedly belongs failed to be cast into a DesignEqualsImplementationClass. This is likely a programming bug on the serialization side of things, but who knows");
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
                DesignEqualsImplementationClass *nonFunctionMemberParentClass = qobject_cast<DesignEqualsImplementationClass*>(projectToPopulate->typeFromSerializedTypeId(classIdofParentThatHasAus));
                int nonFunctionMemberId;
                projectDataStream >> nonFunctionMemberId;
                NonFunctionMember *instanceInOtherClass = nonFunctionMemberParentClass->nonFunctionMemberFromNonFunctionMemberId(nonFunctionMemberId);
                currentClassLifeline->setInstanceInOtherClassIfApplicable(instanceInOtherClass);
            }
            currentClassLifeline->m_ParentUseCase = currentUseCase;

            QList<SerializableSlotIdType> slotsAppearingInClassLifeline;
            projectDataStream >> slotsAppearingInClassLifeline;
            Q_FOREACH(SerializableSlotIdType currentSlotReference, slotsAppearingInClassLifeline)
            {
                //Project Use Class Lifeline Slots
                currentClassLifeline->m_MySlotsAppearingInClassLifeLine.append(qobject_cast<DesignEqualsImplementationClass*>(projectToPopulate->typeFromSerializedTypeId(currentSlotReference.first))->slotInstantiationFromSerializedSlotId(currentSlotReference.second));
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
                IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements *statement;
                switch(static_cast<IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::StatementTypeEnum>(currentStatementType))
                {
                case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::SignalEmitStatementType:
                    statement = new DesignEqualsImplementationSignalEmissionStatement();
                    break;
                case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::SlotInvokeStatementType: //TODOreq: [de-]serialize class lifeline containing slot to invoke
                    statement = new DesignEqualsImplementationSlotInvocationStatement();
                    break;
                case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::PrivateMethodSynchronousCallStatementType:
                    statement = new DesignEqualsImplementationPrivateMethodSynchronousCallStatement();
                    break;
                case IDesignEqualsImplementationStatement_OrChunkOfRawCppStatements::ChunkOfRawCppStatements:
                    statement = new DesignEqualsImplementationChunkOfRawCppStatements();
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
            DesignEqualsImplementationUseCase::SignalSlotConnectionActivationTypeStruct *signalSlotConnectionActivation = new DesignEqualsImplementationUseCase::SignalSlotConnectionActivationTypeStruct();
            //Signal
            //SignalStatement_Key0_IndexInto_m_ClassLifeLines
            int signalStatement_Key0_IndexInto_m_ClassLifeLines;
            projectDataStream >> signalStatement_Key0_IndexInto_m_ClassLifeLines;
            signalSlotConnectionActivation->SignalStatement_Key0_SourceClassLifeLine = currentUseCase->classLifelineInstantiatedFromSerializedClassLifelineId(signalStatement_Key0_IndexInto_m_ClassLifeLines);

            //SignalStatement_Key1_SourceSlotItself
            SerializableSlotIdType serializedSourceSlotId;
            projectDataStream >> serializedSourceSlotId;
            signalSlotConnectionActivation->SignalStatement_Key1_SourceSlotItself = qobject_cast<DesignEqualsImplementationClass*>(currentUseCase->m_DesignEqualsImplementationProject->typeFromSerializedTypeId(serializedSourceSlotId.first))->slotInstantiationFromSerializedSlotId(serializedSourceSlotId.second);

            //SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements
            projectDataStream >> signalSlotConnectionActivation->SignalStatement_Key2_IndexInto_SlotsOrderedListOfStatements;


            //Slots
            int numSlotsAttachedToThisSignalForThisUseCase;
            projectDataStream >> numSlotsAttachedToThisSignalForThisUseCase;
            for(int i = 0; i < numSlotsAttachedToThisSignalForThisUseCase; ++i)
            {
                SlotConnectedToSignalTypedef *currentSlotConnectedToSignal = new SlotConnectedToSignalTypedef();

                //SlotInvokedThroughConnection_Key0_IndexInto_m_ClassLifeLines
                projectDataStream >> currentSlotConnectedToSignal->first;
                //SlotInvokedThroughConnection_Key1_DestinationSlotItself
                SerializableSlotIdType serializedDestinationedSlotId;
                projectDataStream >> serializedDestinationedSlotId;
                currentSlotConnectedToSignal->second = qobject_cast<DesignEqualsImplementationClass*>(currentUseCase->m_DesignEqualsImplementationProject->typeFromSerializedTypeId(serializedDestinationedSlotId.first))->slotInstantiationFromSerializedSlotId(serializedDestinationedSlotId.second); //TODOoptimization: class id is not needed, because class lifeline was just streamed, so can be determined indirectly

                signalSlotConnectionActivation->SlotsAttachedToTheSignal.append(currentSlotConnectedToSignal);
            }

            currentUseCase->m_SignalSlotConnectionActivationsInThisUseCase.append(signalSlotConnectionActivation);
        }
    }
}
