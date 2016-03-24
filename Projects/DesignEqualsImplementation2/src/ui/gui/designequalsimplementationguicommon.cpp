#include "designequalsimplementationguicommon.h"

#include <QMessageBox>

#include "../../designequalsimplementationproject.h"

bool DesignEqualsImplementationGuiCommon::parseNewFunctionDefinitioin_then_askWhatToDoWithNewFunctionArgTypes(QWidget *parentWidget, DesignEqualsImplementationProject *currentProject, const QString &functionDefinitionToParse, QString *out_parsedFunctionName, QList<FunctionArgumentTypedef> *out_parsedFunctionArgs)
{
    DesignEqualsImplementationLenientSignalOrSlotSignaturerParser functionSignatureParser(functionDefinitionToParse, currentProject->allKnownTypesNames());
    if(functionSignatureParser.hasError())
    {
        QMessageBox::critical(parentWidget, QObject::tr("Error"), functionSignatureParser.mostRecentError()); //TODOreq: show the details in-app in a qplaintextedit
        return false;
    }

    //account for any new param type that is unknown to us
    if(!functionSignatureParser.newTypesSeenInFunctionDeclaration().isEmpty())
    {
        NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog(functionSignatureParser.newTypesSeenInFunctionDeclaration(), currentProject, NewTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog::TypesCanBeQObjectDerived, parentWidget); //TODOreq: need more args passed to us to decide if types can or cannot be qobject derived (enum). HOWEVER atm all callers of this method are in fact QObjects (so sub-types can be QObjects)
        if(newTypeSeen_CreateDesignEqualsClassFromIt_OrNoteAsDefinedElsewhereType_dialog.exec() != QDialog::Accepted)
            return false;
    }

    *out_parsedFunctionName = functionSignatureParser.parsedFunctionName();
    *out_parsedFunctionArgs = functionSignatureParser.parsedFunctionArguments();
    return true;
}
bool DesignEqualsImplementationGuiCommon::parseNewSignalDefinition_then_askWhatToDoWithNewSignalArgTypes_then_createNewSignal(QWidget *parentWidget, DesignEqualsImplementationClass *classToAddTheSignalTo, const QString &signalDefinitionToParse)
{
    QString parsedSignalName;
    QList<FunctionArgumentTypedef> parsedSignalArguments;
    if(!parseNewFunctionDefinitioin_then_askWhatToDoWithNewFunctionArgTypes(parentWidget, classToAddTheSignalTo->m_ParentProject, signalDefinitionToParse, &parsedSignalName, &parsedSignalArguments))
        return false;

    //now create the new signal itself
    classToAddTheSignalTo->createNewSignal(parsedSignalName, parsedSignalArguments);
    return true;
}
bool DesignEqualsImplementationGuiCommon::parseNewSlotDefinition_then_askWhatToDoWithNewSlotArgTypes_then_createNewSlot(QWidget *parentWidget, DesignEqualsImplementationClass *classToAddTheSlotTo, const QString &slotDefinitionToParse)
{
    QString parsedSlotName;
    QList<FunctionArgumentTypedef> parsedSlotArguments;
    if(!parseNewFunctionDefinitioin_then_askWhatToDoWithNewFunctionArgTypes(parentWidget, classToAddTheSlotTo->m_ParentProject, slotDefinitionToParse, &parsedSlotName, &parsedSlotArguments))
        return false;

    //now create the new slot itself
    classToAddTheSlotTo->createwNewSlot(parsedSlotName, parsedSlotArguments);
    return true;
}
