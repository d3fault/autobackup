#include "userinterfaceskeletongeneratordata.h"

#include "userinterfaceskeletongenerator.h"

using namespace UserInterfaceSkeletonGeneratorData;

ArgsWithOptionalDefaultValues_List Data::mandatoryArgsListToOptionalArgsList(const ArgsWithMandatoryDefaultValues_List &mandatoryArgsList)
{
    ArgsWithOptionalDefaultValues_List ret;
    for(ArgsWithMandatoryDefaultValues_List::const_iterator it = mandatoryArgsList.constBegin(); it != mandatoryArgsList.constEnd(); ++it)
    {
        const SingleArgWithMandatoryDefaultValue &currentArg = *it;
        ret.append(SingleArgWithOptionalDefaultValue(currentArg.ArgType, currentArg.argName(), *currentArg.ArgOptionalDefaultValue.data()));
    }
    return ret;
}
void Data::createAndAddSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs)
{
    Signals.append(createSignal(signalName, signalArgs));
}
//TODOreq: remove slotReturnType. people who want a slot return type really want RequestResponse. then again I should maintain flexibility and keep it!! hmm but maybe make it the last optional argument and default to of course void. because: QBlockingQueuedConnection fukken worx man and I have no GOOD reason to stop it from working
void Data::createAndAddSlot(QString slotReturnType, QString slotName, ArgsList slotArgs)
{
    Slots.append(createSlot(slotReturnType, slotName, slotArgs));
}
void Data::createAndAddRequestResponse_aka_SlotWithFinishedSignal(QString slotName, ArgsList slotArgs, ArgsWithMandatoryDefaultValues_List signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded, QString signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName)
{    
    RequestResponse_aka_SlotWithFinishedSignal_Data requestResponseData;

    //Slot
    requestResponseData.Slot = createSlot("void", slotName, slotArgs);

    //Finished Signal
    QString signalName = signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName.isEmpty() ? finishedSignalNameFromSlotName(slotName) : signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName;
    ArgsWithOptionalDefaultValues_List argsWithOptionalDefaultValues = Data::mandatoryArgsListToOptionalArgsList(signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded);
    argsWithOptionalDefaultValues.prepend(SingleArgWithOptionalDefaultValue("bool", "success", "false"));
    SignalData signalData = createSignal(signalName, argsWithOptionalDefaultValues);
    requestResponseData.FinishedSignal = signalData;

    RequestResponses_aka_SlotsWithFinishedSignals.append(requestResponseData);
}
QString Data::firstLetterToUpper(const QString &inputString)
{
    if(inputString.isEmpty())
        return inputString;
    QString ret(inputString);
    ret.replace(0, 1, ret.at(0).toUpper());
    return ret;
}
QString Data::finishedSignalNameFromSlotName(QString slotName)
{
    return slotName + "Finished";
}
SignalData Data::createSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs)
{
    SignalData signalData;
    signalData.setSignalName(signalName);
    signalData.setSignalArgs(signalArgs);
    return signalData;
}
SlotData Data::createSlot(QString slotReturnType, QString slotName, ArgsList slotArgs)
{
    SlotData slotData;
    slotData.SlotReturnType = slotReturnType;
    slotData.setSlotName(slotName);
    slotData.setSlotArgs(slotArgs);
    return slotData;
}
#if 0 //fuck virtual inheritance interfaces in this case
void UserInterfaceSkeletonGeneratorData::generatePureVirtualUserInterfaceHeaderFile(QTextStream &textStream) const
{
    textStream << "#include <QString>" << endl << endl; //TODOreq: auto-include based on arg types and slot return types... ffffff....

    textStream << "class QObject;" << endl << endl;

    textStream << "class " << targetUserInterfaceClassName() << endl << "{" << endl;

    textStream << "public:" << endl;
    textStream << UserInterfaceSkeletonGenerator::TAB << targetUserInterfaceClassName() << "()=default;" << endl;
    textStream << UserInterfaceSkeletonGenerator::TAB << targetUserInterfaceClassName() << "(const " << targetUserInterfaceClassName() << " &other)=delete;" << endl;
    textStream << UserInterfaceSkeletonGenerator::TAB << "virtual ~" << targetUserInterfaceClassName() << "()=default;" << endl << endl;

    textStream << UserInterfaceSkeletonGenerator::TAB << "virtual QObject *asQObject()=0;" << endl;

    if(!Slots.isEmpty())
        textStream << "protected: //signals:" << endl;
    Q_FOREACH(SlotData slotData, Slots)
    {
        textStream << UserInterfaceSkeletonGenerator::TAB << "virtual void " /*always void because is signal! not to be confused with SlotReturnType, which comes back as an ARG of this signal*/ << slotData.correspondingRequestSignalName() + slotData.argsWithParenthesis(slotData.slotArgs()) << "=0;" << endl;
        //TODOreq: gen a slot called "handleSlotNameFinished", and if slotData.SlotReturnType is non-void, an arg of that type (called something generic) should be it's only arg. maybe actually I should use bool always as first param (bool success), and the return type specified is an OPTIONAL 2nd arg following bool success? actually yea I like this idea better!!! TODOreq
    }

    textStream << "};" << endl;

    //TODOreq: header guards
}
QString UserInterfaceSkeletonGeneratorData::targetUserInterfaceClassName() const
{
    return QString("I" + BusinessLogicClassName + "UI"); //"UserInterface" suffix too verbose, esp since "I" is at beginning as well xDDDDD. even though I know the 'I' at beginning stands for interface, I tend to read it as "I am a Business Logic Class Name UI", the 'I' taking on a different meaning than "Interface" there (and it helps with udnerstandability imo)
}
#endif
QString IFunctionSignatureWithoutReturnType::argsWithoutParenthesis(ArgPtrsList functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const
{
    QString ret;
    bool first = true;
    Q_FOREACH(const SingleArg *currentArg, functionArgs)
    {
        if(!first)
            ret.append(", ");
        first = false;

        ret.append(currentArg->ArgType + " " + currentArg->argName(displayArgInitialization)); //TODOmb: if argType ends with "reference amp" or ptr asterisk, don't use a space. steal this code from d=i pls
    }
    return ret;
}

QString IFunctionSignatureWithoutReturnType::argsWithParenthesis(ArgPtrsList functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const
{
    QString ret("(");
    ret.append(argsWithoutParenthesis(functionArgs, displayArgInitialization));
    ret.append(")");
    return ret;
}
QString IFunctionSignatureWithoutReturnType::argsWithParenthesis(const ArgsList &functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const
{
    ArgPtrsList argPtrsList;
    Q_FOREACH(const SingleArg &singleArg, functionArgs)
    {
        argPtrsList.append(&singleArg);
    }
    return argsWithParenthesis(argPtrsList, displayArgInitialization);
}
QString IFunctionSignatureWithoutReturnType::argsWithParenthesis(const ArgsWithOptionalDefaultValues_List &functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const
{
    ArgPtrsList argPtrsList;
    Q_FOREACH(const SingleArgWithOptionalDefaultValue &singleArg, functionArgs)
    {
        argPtrsList.append(&singleArg);
    }
    return argsWithParenthesis(argPtrsList, displayArgInitialization);
}
QString IFunctionSignatureWithoutReturnType::argsWithParenthesis(const ArgsWithMandatoryDefaultValues_List &functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const
{
    ArgPtrsList argPtrsList;
    Q_FOREACH(const SingleArgWithMandatoryDefaultValue &singleArg, functionArgs)
    {
        argPtrsList.append(&singleArg);
    }
    return argsWithParenthesis(argPtrsList, displayArgInitialization);
}
#if 0 //Qt4-style connect
QString myNormalizedType(QString input)
{
    const QByteArray &inputAsUtf8 = input.toUtf8();
    QByteArray retBA = QMetaObject::normalizedType(inputAsUtf8.constData());
    QString ret(retBA);
    return ret;
}
QString UserInterfaceSkeletonGeneratorIFunctionSignatureWithoutReturnType::argTypesNormalizedWithoutParenthesis() const
{
    QString ret;
    bool first = true;
    Q_FOREACH(SingleArgWithoutDefaultValue currentArg, FunctionArgs)
    {
        if(!first)
            ret.append(",");
        first = false;

        ret.append(myNormalizedType(currentArg.ArgType));
    }
    return ret;
}
QString UserInterfaceSkeletonGeneratorIFunctionSignatureWithoutReturnType::argTypesNormalizedAndWithParenthesis() const
{
    QString ret("(");
    ret.append(argTypesNormalizedWithoutParenthesis());
    ret.append(")");
    return ret;
}
#endif
QString SlotData::correspondingSlotInvokeRequestSignalName() const
{
    return slotName() + "Requested";
}
QString SignalData::correspondingSignalHandlerSlotName() const
{
    return "handle" + Data::firstLetterToUpper(signalName());
}
QString RequestResponse_aka_SlotWithFinishedSignal_Data::slotScopedResponderTypeName() const
{
    //ex: SomeSlotScopedResponder
    QString ret(Data::firstLetterToUpper(Slot.slotName()));
    ret.append("ScopedResponder");
    return ret;
}
QString RequestResponse_aka_SlotWithFinishedSignal_Data::slotRequestResponseTypeName() const
{
    //ex: SomeSlotRequestResponse
    QString ret(Data::firstLetterToUpper(Slot.slotName()));
    ret.append("RequestResponse");
    return ret;
}
QString SingleArg::argName(SingleArg::DisplayArgInitializationEnum displayArgInitializationEnum) const
{
    Q_UNUSED(displayArgInitializationEnum);
    return m_ArgName;
}
QString SingleArgWithOptionalDefaultValue::argName(SingleArgWithOptionalDefaultValue::DisplayArgInitializationEnum displayArgInitializationEnum) const
{
    QString ret = SingleArg::argName();
    switch(displayArgInitializationEnum)
    {
        case DoNotDisplayArgInitialization:
            //do nothing
        break;
        case DisplayConstructorStyleInitialization:
        {
            if(!ArgOptionalDefaultValue.isNull())
            {
                ret += "(" + (*ArgOptionalDefaultValue.data()) + ")";
            }
        }
        break;
        case DisplaySimpleAssignmentStyleInitialization:
        {
            if(!ArgOptionalDefaultValue.isNull())
            {
                ret.append(" = " + (*ArgOptionalDefaultValue.data()));
            }
        }
        break;
    }
    return ret;
}
