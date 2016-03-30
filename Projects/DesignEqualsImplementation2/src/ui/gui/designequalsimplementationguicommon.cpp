#include "designequalsimplementationguicommon.h"

#include <QMessageBox>

#include "../../designequalsimplementationproject.h"

bool DesignEqualsImplementationGuiCommon::parseNewFunctionDefinitioin_then_askWhatToDoWithNewFunctionArgTypes(QWidget *parentWidget, DesignEqualsImplementationProject *currentProject, const QString &functionDefinitionToParse, QString *out_parsedFunctionName, QList<ParsedTypeInstance> *out_parsedFunctionArgs)
{
    LibClangFunctionDeclarationParser functionSignatureParser(functionDefinitionToParse, currentProject->allKnownTypesNames());
    if(functionSignatureParser.hasError())
    {
        QMessageBox::critical(parentWidget, QObject::tr("Error"), functionSignatureParser.mostRecentError()); //TODOreq: show the details in-app in a qplaintextedit
        return false;
    }

#if 0 //KISS. herein is the PROPER way, but eh /lazy etc
    QList<TypeInstance*> realTypeInstances;
    if(!convertAllParsedTypeInstancesIntoRealTypeInstances(functionSignatureParser.parsedFunctionArguments(), &realTypeInstances))
        return false;
#else
    currentProject->ensureParsedBuiltInTypesHaveTypes(functionSignatureParser.parsedFunctionArguments());
#endif

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
#if 0
bool DesignEqualsImplementationGuiCommon::convertAllParsedTypeInstancesIntoRealTypeInstances(const QList<ParsedTypeInstance> &parsedTypeInstances, DesignEqualsImplementationProject *project, QList<TypeInstance *> *out_RealTypeInstances)
{
    //re: parsed type instances
    //-for each builtin, getOrCreateBuiltInType
    //-for each known, getTypeByName
    //-for each unknown, ask user whether Class/ImplicitlyShared/DefinedElsewhere

    //TODOreq: we want to retain ordering

    bool needUserInput = false;
    Q_FOREACH(const ParsedTypeInstance &currentParsedType, parsedTypeInstances)
    {
        DesignEqualsImplementationType *type;
        switch(currentParsedType.ParsedTypeInstanceCategory)
        {
        case ParsedTypeInstance::KnownTypeButNotABuiltIn:
        case ParsedTypeInstance::Unknown: //we made them known just a few statements (had:seconds) ago!
            type = project->getTypeFromName(currentParsedType.NonQualifiedType);
        break;
        case ParsedTypeInstance::BuiltIn:
            type = project->getOrCreateBuiltInType(currentParsedType.NonQualifiedType);
        break;
        }
        out_RealTypeInstances->append();
    }
}
#endif
bool DesignEqualsImplementationGuiCommon::parseNewSignalDefinition_then_askWhatToDoWithNewSignalArgTypes_then_createNewSignal(QWidget *parentWidget, DesignEqualsImplementationClass *classToAddTheSignalTo, const QString &signalDefinitionToParse)
{
    QString parsedSignalName;
    QList<ParsedTypeInstance> parsedSignalArguments;
    if(!parseNewFunctionDefinitioin_then_askWhatToDoWithNewFunctionArgTypes(parentWidget, classToAddTheSignalTo->m_ParentProject, signalDefinitionToParse, &parsedSignalName, &parsedSignalArguments))
        return false;

    //now create the new signal itself
    classToAddTheSignalTo->createNewSignal(parsedSignalName, parsedSignalArguments);
    return true;
}
bool DesignEqualsImplementationGuiCommon::parseNewSlotDefinition_then_askWhatToDoWithNewSlotArgTypes_then_createNewSlot(QWidget *parentWidget, DesignEqualsImplementationClass *classToAddTheSlotTo, const QString &slotDefinitionToParse)
{
    QString parsedSlotName;
    QList<ParsedTypeInstance> parsedSlotArguments;
    if(!parseNewFunctionDefinitioin_then_askWhatToDoWithNewFunctionArgTypes(parentWidget, classToAddTheSlotTo->m_ParentProject, slotDefinitionToParse, &parsedSlotName, &parsedSlotArguments))
        return false;

    //now create the new slot itself
    classToAddTheSlotTo->createwNewSlot(parsedSlotName, parsedSlotArguments);
    return true;
}
