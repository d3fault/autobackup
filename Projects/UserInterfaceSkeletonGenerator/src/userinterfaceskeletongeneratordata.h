#ifndef USERINTERFACESKELETONGENERATORDATA_H
#define USERINTERFACESKELETONGENERATORDATA_H

#include <QTextStream>
#include <QScopedPointer>

struct UserInterfaceSkeletonGeneratorData
{
    struct SingleArgWithoutDefaultValue //slots (no default values allowed)
    {
        SingleArgWithoutDefaultValue(QString argType, QString argName)
            : ArgType(argType)
            , ArgName(argName)
        { }
        QString ArgType;
        QString ArgName;
    };
    typedef QList<SingleArgWithoutDefaultValue> ArgsWithoutDefaultValues_List;

    struct SingleArgWithOptionalDefaultValue : public SingleArgWithoutDefaultValue //signals (default values optional)
    {
        SingleArgWithOptionalDefaultValue(QString argType, QString argName)
            : SingleArgWithoutDefaultValue(argType, argName)
        { }
        SingleArgWithOptionalDefaultValue(QString argType, QString argName, QString argDefaultValue)
            : SingleArgWithoutDefaultValue(argType, argName)
            , ArgDefaultValue(new QString(argDefaultValue))
        { }
        SingleArgWithOptionalDefaultValue(const SingleArgWithoutDefaultValue &other)
            : SingleArgWithoutDefaultValue(other.ArgType, other.ArgName)
        { }
        SingleArgWithOptionalDefaultValue(const SingleArgWithOptionalDefaultValue &other)
            : SingleArgWithoutDefaultValue(other.ArgType, other.ArgName)
        {
            if(!other.ArgDefaultValue.isNull())
                ArgDefaultValue.reset(new QString(*other.ArgDefaultValue.data()));
        }
#if 0
        SingleArgWithOptionalDefaultValue(const SingleArgWithMandatoryDefaultValue &other)
            : SingleArgWithoutDefaultValue(other.ArgType, other.ArgName)
        {
            if(!other.ArgDefaultValue.isNull())
                ArgDefaultValue.reset(new QString(other.ArgDefaultValue.data()));
        }
#endif
        void operator=(const SingleArgWithoutDefaultValue &other)
        {
            ArgType = other.ArgType;
            ArgName = other.ArgName;
            ArgDefaultValue.reset();
        }

        QScopedPointer<QString> ArgDefaultValue;
    };
    typedef QList<SingleArgWithOptionalDefaultValue> ArgsWithOptionalDefaultValues_List;

    struct SingleArgWithMandatoryDefaultValue : public SingleArgWithOptionalDefaultValue //signals in RequestResponseContract_aka_slotWithFinishedSignal (default values MANDATORY)
    {
        SingleArgWithMandatoryDefaultValue(QString argType, QString argName)=delete;
        SingleArgWithOptionalDefaultValue toSingleArgWithOptionalDefaultValue() const
        {
            SingleArgWithOptionalDefaultValue ret(ArgType, ArgName);
            if(!ArgDefaultValue.isNull())
                ret.ArgDefaultValue.reset(new QString(*ArgDefaultValue.data()));
            return ret;
        }
    };
    typedef QList<SingleArgWithMandatoryDefaultValue> ArgsWithMandatoryDefaultValues_List;

    class IFunctionSignatureWithoutReturnType //because signal doesn't have return type
    {
    public:
        QString argsWithoutParenthesis(bool showDefaultValueInit = false) const;
        QString argsWithParenthesis() const;
#if 0 //Qt4-style connect
        QString argTypesNormalizedWithoutParenthesis() const;
        QString argTypesNormalizedAndWithParenthesis() const;
#endif
    protected:
        QString FunctionName;
        ArgsWithOptionalDefaultValues_List FunctionArgs;
    };
    struct SignalData : public IFunctionSignatureWithoutReturnType
    {
        //signal return type ALWAYS void

        void setSignalName(const QString &signalName) { FunctionName = signalName; }
        QString signalName() const { return FunctionName; }
        void setSignalArgs(const ArgsWithOptionalDefaultValues_List &signalArgs) { FunctionArgs = signalArgs; }
        ArgsWithOptionalDefaultValues_List signalArgs() const { return FunctionArgs; }

        QString correspondingSignalHandlerSlotName() const;
    };
    struct SlotData : public IFunctionSignatureWithoutReturnType
    {
        QString SlotReturnType; //should be void, but doesn't have to be

        void setSlotName(const QString &slotName) { FunctionName = slotName; }
        QString slotName() const { return FunctionName; }
        void setSlotArgs(const ArgsWithoutDefaultValues_List &slotArgs) { FunctionArgs = slotArgs; }
        ArgsWithoutDefaultValues_List slotArgs() const { return FunctionArgs; }

        QString correspondingSlotInvokeRequestSignalName() const;
    };

    struct RequestResponse_aka_SlotWithFinishedSignal_Data
    {
        SlotData Slot;
        SignalData FinishedSignal;
    };

    void createAndAddSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs = ArgsWithOptionalDefaultValues_List());
    void createAndAddSlot(QString slotReturnType /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsWithoutDefaultValues_List slotArgs = ArgsWithoutDefaultValues_List());

    void createAndAddRequestResponse_aka_SlotWithFinishedSignal(QString slotName, ArgsWithoutDefaultValues_List slotArgs = ArgsWithoutDefaultValues_List(), ArgsWithMandatoryDefaultValues_List signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded = ArgsWithMandatoryDefaultValues_List(), QString signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName = QString());

    //void generatePureVirtualUserInterfaceHeaderFile(QTextStream &textStream) const;

    //QString targetUserInterfaceClassName() const;

    QString BusinessLogiClassName;

    //Basic Qt MetaMethods
    QList<SignalData> Signals;
    QList<SlotData> Slots;

    //Exotic SlotWithFinishedSignal Contract Thingo
    QList<RequestResponse_aka_SlotWithFinishedSignal_Data> RequestResponses_aka_SlotsWithFinishedSignals;
private:
    static QString firstLetterToUpper(const QString &inputString);

    static SignalData createSignal(QString signalName, ArgsWithOptionalDefaultValues_List signalArgs);
    static SlotData createSlot(QString slotReturnType, QString slotName, ArgsWithoutDefaultValues_List slotArgs);
};

#endif // USERINTERFACESKELETONGENERATORDATA_H
