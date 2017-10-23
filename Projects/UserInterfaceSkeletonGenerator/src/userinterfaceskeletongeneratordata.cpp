#include "userinterfaceskeletongeneratordata.h"

#include "userinterfaceskeletongenerator.h"

void UserInterfaceSkeletonGeneratorData::createAndAddSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs)
{
    Signals.append(createSignal(signalName, signalArgs));
}
//TODOreq: remove slotReturnType. people who want a slot return type really want RequestResponse. then again I should maintain flexibility and keep it!! hmm but maybe make it the last optional argument and default to of course void. because: QBlockingQueuedConnection fukken worx man and I have no GOOD reason to stop it from working
void UserInterfaceSkeletonGeneratorData::createAndAddSlot(QString slotReturnType, QString slotName, ArgsWithoutDefaultValues_List slotArgs)
{
    Slots.append(createSlot(slotReturnType, slotName, slotArgs));
}
void UserInterfaceSkeletonGeneratorData::createAndAddRequestResponse_aka_SlotWithFinishedSignal(QString slotName, UserInterfaceSkeletonGeneratorData::ArgsWithoutDefaultValues_List slotArgs, ArgsWithMandatoryDefaultValues_List signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded, QString signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName)
{    
    RequestResponse_aka_SlotWithFinishedSignal_Data requestResponseData;

    //Slot
    requestResponseData.Slot = createSlot("void", slotName, slotArgs);

    //Finished Signal
    QString signalName = signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName; //TODOreq
    SignalData signalData = createSignal(signalName, signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded);
    ArgsWithoutDefaultValues_List signalArgs = signalData.signalArgs();
    signalArgs.prepend(createArg("bool", "success"));
    signalData.setSignalArgs(signalArgs);
    requestResponseData.FinishedSignal = signalData;
}
QString UserInterfaceSkeletonGeneratorData::firstLetterToUpper(const QString &inputString)
{
    if(inputString.isEmpty())
        return inputString;
    QString ret(inputString);
    ret.replace(0, 1, ret.at(0).toUpper());
    return ret;
}
UserInterfaceSkeletonGeneratorData::SignalData UserInterfaceSkeletonGeneratorData::createSignal(QString signalName, UserInterfaceSkeletonGeneratorData::ArgsWithOptionalDefaultValues_List signalArgs)
{
    SignalData signalData;
    signalData.setSignalName(signalName);
    signalData.setSignalArgs(signalArgs);
    return signalData;
}
UserInterfaceSkeletonGeneratorData::SlotData UserInterfaceSkeletonGeneratorData::createSlot(QString slotReturnType, QString slotName, UserInterfaceSkeletonGeneratorData::ArgsWithoutDefaultValues_List slotArgs)
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
        textStream << UserInterfaceSkeletonGenerator::TAB << "virtual void " /*always void because is signal! not to be confused with SlotReturnType, which comes back as an ARG of this signal*/ << slotData.correspondingRequestSignalName() + slotData.argsWithParenthesis() << "=0;" << endl;
        //TODOreq: gen a slot called "handleSlotNameFinished", and if slotData.SlotReturnType is non-void, an arg of that type (called something generic) should be it's only arg. maybe actually I should use bool always as first param (bool success), and the return type specified is an OPTIONAL 2nd arg following bool success? actually yea I like this idea better!!! TODOreq
    }

    textStream << "};" << endl;

    //TODOreq: header guards
}
QString UserInterfaceSkeletonGeneratorData::targetUserInterfaceClassName() const
{
    return QString("I" + BusinessLogiClassName + "UI"); //"UserInterface" suffix too verbose, esp since "I" is at beginning as well xDDDDD. even though I know the 'I' at beginning stands for interface, I tend to read it as "I am a Business Logic Class Name UI", the 'I' taking on a different meaning than "Interface" there (and it helps with udnerstandability imo)
}
#endif
QString UserInterfaceSkeletonGeneratorData::IFunctionSignatureWithoutReturnType::argsWithoutParenthesis(bool showDefaultValueInit) const
{
    QString ret;
    bool first = true;
    Q_FOREACH(const SingleArgWithOptionalDefaultValue &currentArg, FunctionArgs)
    {
        if(!first)
            ret.append(", ");
        first = false;

        ret.append(currentArg.ArgType + " " + currentArg.ArgName); //TODOmb: if argType ends with "reference amp" or ptr asterisk, don't use a space. steal this code from d=i pls
        if(showDefaultValueInit && (!currentArg.ArgDefaultValue.isNull()))
        {
            ret.append(" = " + currentArg.ArgDefaultValue.data());
        }
    }
    return ret;
}

QString UserInterfaceSkeletonGeneratorData::IFunctionSignatureWithoutReturnType::argsWithParenthesis() const
{
    QString ret("(");
    ret.append(argsWithoutParenthesis());
    ret.append(")");
    return ret;
}
QString myNormalizedType(QString input)
{
    const QByteArray &inputAsUtf8 = input.toUtf8();
    QByteArray retBA = QMetaObject::normalizedType(inputAsUtf8.constData());
    QString ret(retBA);
    return ret;
}
#if 0 //Qt4-style connect
QString UserInterfaceSkeletonGeneratorData::IFunctionSignatureWithoutReturnType::argTypesNormalizedWithoutParenthesis() const
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
QString UserInterfaceSkeletonGeneratorData::IFunctionSignatureWithoutReturnType::argTypesNormalizedAndWithParenthesis() const
{
    QString ret("(");
    ret.append(argTypesNormalizedWithoutParenthesis());
    ret.append(")");
    return ret;
}
#endif
QString UserInterfaceSkeletonGeneratorData::SlotData::correspondingSlotInvokeRequestSignalName() const
{
    return slotName() + "Requested";
}
QString UserInterfaceSkeletonGeneratorData::SignalData::correspondingSignalHandlerSlotName() const
{
    return "handle" + firstLetterToUpper(signalName());
}
