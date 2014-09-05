#include "designequalsimplementationprojectserializer.h"

#include <QDataStream>

#include "designequalsimplementationproject.h"
#include "designequalsimplementationsignalemissionstatement.h"
#include "designequalsimplementationprivatemethodsynchronouscallstatement.h"
#include "designequalsimplementationslotinvocationstatement.h"

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
DesignEqualsImplementationProjectSerializer::DesignEqualsImplementationProjectSerializer(QObject *parent)
    : QObject(parent)
{ }
//save
void DesignEqualsImplementationProjectSerializer::serializeProjectToIoDevice(DesignEqualsImplementationProject *projectToSerialize, QIODevice *ioDeviceToSerializeTo)
{
    QDataStream projectDataStream(ioDeviceToSerializeTo);
    //projectDataStream << *projectToSerialize;

    quint8 serializationVersion = 1;
    projectDataStream << serializationVersion;

    //Project
    projectDataStream << projectToSerialize->Name;
    projectDataStream << projectToSerialize->m_Classes.size();
    Q_FOREACH(DesignEqualsImplementationClass *currentClass, projectToSerialize->m_Classes)
    {
        //Project Classes
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

            projectDataStream << currentSlot->m_OrderedListOfStatements.size();
            Q_FOREACH(IDesignEqualsImplementationStatement *currentStatement, currentSlot->m_OrderedListOfStatements)
            {
                //Project Class Slots Statements
                projectDataStream << static_cast<quint8>(currentStatement->StatementType);
                currentStatement->streamOut(projectDataStream);
            }
        }
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
        //Project Classes
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
            DesignEqualsImplementationClassSlot *currentSlot = currentClass->createwNewSlot(slotName, methodArguments);

            int numOrderedListOfStatements;
            projectDataStream >> numOrderedListOfStatements;
            for(int k = 0; k < numOrderedListOfStatements; ++k)
            {
                //Project Class Slots Statements
                quint8 currentStatementType;
                projectDataStream >> currentStatementType;
                IDesignEqualsImplementationStatement *statement;
                switch(currentStatementType)
                {
                case IDesignEqualsImplementationStatement::SignalEmitStatementType:
                    statement = new DesignEqualsImplementationSignalEmissionStatement();
                    break;
                case IDesignEqualsImplementationStatement::SlotInvokeStatementType:
                    statement = new DesignEqualsImplementationSlotInvocationStatement();
                    break;
                case IDesignEqualsImplementationStatement::PrivateMethodSynchronousCallStatementType:
                    statement = new DesignEqualsImplementationPrivateMethodSynchronousCallStatement();
                    break;
                }
                statement->streamIn(projectDataStream);
                currentSlot->m_OrderedListOfStatements.append(statement);
            }
        }
    }
}
