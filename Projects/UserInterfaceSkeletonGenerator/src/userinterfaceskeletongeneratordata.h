#ifndef USERINTERFACESKELETONGENERATORDATA_H
#define USERINTERFACESKELETONGENERATORDATA_H

#include <QTextStream>
#include <QSharedPointer>

namespace UserInterfaceSkeletonGeneratorData
{

//slots = no default values allowed
//signals = default values optional
//requestResponseContractFinishedSignal = default values mandatory (at least initially. it's just safer and allows for lazier coding/thinking. I should provide a way, preferably via compile time switch for best speed gains, to disable default value [re]setting... but I think that should be opted into! there might be cases where a runtime switch would be more appropriate, each RequestResponse (slotWithFinishedSignal) gets it's own m_ResetDefaultValuesAfterResponseAkaEmit)
struct SingleArg
{
    SingleArg(QString argType, QString argName)
        : ArgType(argType)
        , m_ArgName(argName)
    { }
    virtual ~SingleArg()=default;
    QString ArgType;
    enum DisplayArgInitializationEnum
    {
          DoNotDisplayArgInitialization = 0
        , DisplayConstructorStyleInitialization = 1
        , DisplaySimpleAssignmentStyleInitialization = 2
    };

    virtual QString argName(DisplayArgInitializationEnum displayArgInitializationEnum = DoNotDisplayArgInitialization) const;
private:
    QString m_ArgName;
};
typedef QList<SingleArg> ArgsList;
typedef QList<const SingleArg*> ArgPtrsList;

struct SingleArgWithOptionalDefaultValue : public SingleArg
{
    SingleArgWithOptionalDefaultValue(QString argType, QString argName)
        : SingleArg(argType, argName)
    { }
    SingleArgWithOptionalDefaultValue(QString argType, QString argName, QString argDefaultValue)
        : SingleArg(argType, argName)
        , ArgOptionalDefaultValue(new QString(argDefaultValue))
    { }
    virtual ~SingleArgWithOptionalDefaultValue()=default;
    QString argName(DisplayArgInitializationEnum displayArgInitializationEnum = DoNotDisplayArgInitialization) const override;
    QSharedPointer<QString> ArgOptionalDefaultValue;
};
typedef QList<SingleArgWithOptionalDefaultValue> ArgsWithOptionalDefaultValues_List;

struct SingleArgWithMandatoryDefaultValue : public SingleArgWithOptionalDefaultValue
{
    SingleArgWithMandatoryDefaultValue(QString argType, QString argName)=delete;
    SingleArgWithMandatoryDefaultValue(QString argType, QString argName, QString argMandatoryDefaultValue)
        : SingleArgWithOptionalDefaultValue(argType, argName, argMandatoryDefaultValue)
    { }
    virtual ~SingleArgWithMandatoryDefaultValue()=default;
};
typedef QList<SingleArgWithMandatoryDefaultValue> ArgsWithMandatoryDefaultValues_List;

class IFunctionSignatureWithoutReturnType //because signal doesn't have return type
{
public:
    QString argsWithoutParenthesis(ArgPtrsList functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const;
    QString argsWithParenthesis(ArgPtrsList functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization) const;
    QString argsWithParenthesis(const ArgsList &functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization = SingleArg::DoNotDisplayArgInitialization) const;
    QString argsWithParenthesis(const ArgsWithOptionalDefaultValues_List &functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization = SingleArg::DoNotDisplayArgInitialization) const;
    QString argsWithParenthesis(const ArgsWithMandatoryDefaultValues_List &functionArgs, SingleArg::DisplayArgInitializationEnum displayArgInitialization = SingleArg::DoNotDisplayArgInitialization) const;
#if 0 //Qt4-style connect
    QString argTypesNormalizedWithoutParenthesis() const;
    QString argTypesNormalizedAndWithParenthesis() const;
#endif
protected:
    QString FunctionName;
    //ArgsList FunctionArgs;
};
struct SignalData : public IFunctionSignatureWithoutReturnType
{
    //signal return type ALWAYS void

    void setSignalName(const QString &signalName) { FunctionName = signalName; }
    QString signalName() const { return FunctionName; }
    void setSignalArgs(const ArgsWithOptionalDefaultValues_List &signalArgs) { SignalArgs = signalArgs; }
    ArgsWithOptionalDefaultValues_List signalArgs() const { return SignalArgs; }

    QString correspondingSignalHandlerSlotName() const;

    ArgsWithOptionalDefaultValues_List SignalArgs;
};
struct SlotData : public IFunctionSignatureWithoutReturnType
{
    QString SlotReturnType; //should be void, but doesn't have to be

    void setSlotName(const QString &slotName) { FunctionName = slotName; }
    QString slotName() const { return FunctionName; }
    void setSlotArgs(const ArgsList &slotArgs) { SlotArgs = slotArgs; }
    ArgsList slotArgs() const { return SlotArgs; }

    QString correspondingSlotInvokeRequestSignalName() const;

    ArgsList SlotArgs;
};

struct RequestResponse_aka_SlotWithFinishedSignal_Data
{
    SlotData Slot;
    SignalData FinishedSignal;

    //helpers
    QString slotScopedResponderTypeName() const;
    QString slotRequestResponseTypeName() const;
};

struct Data
{
    static ArgsWithOptionalDefaultValues_List mandatoryArgsListToOptionalArgsList(const ArgsWithMandatoryDefaultValues_List &mandatoryArgsList);

    void createAndAddSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs = ArgsWithOptionalDefaultValues_List());
    void createAndAddSlot(QString slotReturnType /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsList slotArgs = ArgsList());

    void createAndAddRequestResponse_aka_SlotWithFinishedSignal(QString slotName, ArgsList slotArgs = ArgsList(), ArgsWithMandatoryDefaultValues_List signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded = ArgsWithMandatoryDefaultValues_List(), QString signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName = QString());

    //void generatePureVirtualUserInterfaceHeaderFile(QTextStream &textStream) const;

    //QString targetUserInterfaceClassName() const;

    QString BusinessLogicClassName;

    //Basic Qt MetaMethods
    QList<SignalData> Signals;
    QList<SlotData> Slots;

    //Exotic SlotWithFinishedSignal Contract Thingo
    QList<RequestResponse_aka_SlotWithFinishedSignal_Data> RequestResponses_aka_SlotsWithFinishedSignals;

    static QString firstLetterToUpper(const QString &inputString);
private:
    static QString finishedSignalNameFromSlotName(QString slotName);

    static SignalData createSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs);
    static SlotData createSlot(QString slotReturnType, QString slotName, ArgsList slotArgs);
};

}

#endif // USERINTERFACESKELETONGENERATORDATA_H
