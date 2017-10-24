#ifndef USERINTERFACESKELETONGENERATORDATA_H
#define USERINTERFACESKELETONGENERATORDATA_H

#include <QTextStream>
#include <QSharedPointer>


struct UserInterfaceSkeletonGeneratorData
{
    struct SingleArg //slots (no default values allowed)
    {
        SingleArg(QString argType, QString argName)
            : ArgType(argType)
            , ArgName(argName)
        { }
        QString ArgType;
        QString ArgName;
        QSharedPointer<QString> ArgOptionalDefaultValue;
    };
    typedef QList<SingleArg> ArgsList;

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
        ArgsList FunctionArgs;
    };
    struct SignalData : public IFunctionSignatureWithoutReturnType
    {
        //signal return type ALWAYS void

        void setSignalName(const QString &signalName) { FunctionName = signalName; }
        QString signalName() const { return FunctionName; }
        void setSignalArgs(const ArgsList &signalArgs) { FunctionArgs = signalArgs; }
        ArgsList signalArgs() const { return FunctionArgs; }

        QString correspondingSignalHandlerSlotName() const;
    };
    struct SlotData : public IFunctionSignatureWithoutReturnType
    {
        QString SlotReturnType; //should be void, but doesn't have to be

        void setSlotName(const QString &slotName) { FunctionName = slotName; }
        QString slotName() const { return FunctionName; }
        void setSlotArgs(const ArgsList &slotArgs) { FunctionArgs = slotArgs; }
        ArgsList slotArgs() const { return FunctionArgs; }

        QString correspondingSlotInvokeRequestSignalName() const;
    };

    struct RequestResponse_aka_SlotWithFinishedSignal_Data
    {
        SlotData Slot;
        SignalData FinishedSignal;

        //helpers
        QString slotScopedResponderTypeName() const;
        QString slotRequestResponseTypeName() const;
    };

    void createAndAddSignal(QString signalName, ArgsList signalArgs = ArgsList());
    void createAndAddSlot(QString slotReturnType /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsList slotArgs = ArgsList());

    void createAndAddRequestResponse_aka_SlotWithFinishedSignal(QString slotName, ArgsList slotArgs = ArgsList(), ArgsList signalArgsAllHavingDefaultValues_inAdditionToSuccessBooleanThatWillBeAutoAdded = ArgsList(), QString signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName = QString());

    //void generatePureVirtualUserInterfaceHeaderFile(QTextStream &textStream) const;

    //QString targetUserInterfaceClassName() const;

    QString BusinessLogicClassName;

    //Basic Qt MetaMethods
    QList<SignalData> Signals;
    QList<SlotData> Slots;

    //Exotic SlotWithFinishedSignal Contract Thingo
    QList<RequestResponse_aka_SlotWithFinishedSignal_Data> RequestResponses_aka_SlotsWithFinishedSignals;
private:
    static QString firstLetterToUpper(const QString &inputString);
    static QString finishedSignalNameFromSlotName(QString slotName);

    static SignalData createSignal(QString signalName, ArgsList signalArgs);
    static SlotData createSlot(QString slotReturnType, QString slotName, ArgsList slotArgs);
};

#endif // USERINTERFACESKELETONGENERATORDATA_H
