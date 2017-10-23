#ifndef USERINTERFACESKELETONGENERATORDATA_H
#define USERINTERFACESKELETONGENERATORDATA_H

#include <QTextStream>

struct UserInterfaceSkeletonGeneratorData
{
    struct SingleArg
    {
        QString ArgType;
        QString ArgName;

        //nvm: I was thinking about adding this, but since it would spill over to BusinessClass's slots, I won't. I only want it for "requested signals" emitting, but Qt5 style connects HATE slot overloading, so it's best avoided: QString OptionalDefaultValueAkaInit;
    };
    typedef QList<SingleArg> ArgsList;

    class IClassMethodWithoutReturnType //because signal doesn't have return type
    {
    public:
        QString argsWithoutParenthesis() const;
        QString argsWithParenthesis() const;
        QString argTypesNormalizedWithoutParenthesis() const;
        QString argTypesNormalizedAndWithParenthesis() const;
    protected:
        QString MethodName;
        ArgsList MethodArgs;
    };
    struct SignalData : public IClassMethodWithoutReturnType
    {
        //signal return type ALWAYS void

        void setSignalName(const QString &signalName) { MethodName = signalName; }
        QString signalName() const { return MethodName; }
        void setSignalArgs(const ArgsList &signalArgs) { MethodArgs = signalArgs; }
        ArgsList signalArgs() const { return MethodArgs; }

        QString correspondingSignalHandlerSlotName() const;
    };
    struct SlotData : public IClassMethodWithoutReturnType
    {
        QString SlotReturnType; //should be void, but doesn't have to be

        void setSlotName(const QString &slotName) { MethodName = slotName; }
        QString slotName() const { return MethodName; }
        void setSlotArgs(const ArgsList &slotArgs) { MethodArgs = slotArgs; }
        ArgsList slotArgs() const { return MethodArgs; }

        QString correspondingSlotInvokeRequestSignalName() const;
    };

    struct RequestResponse_aka_SlotWithFinishedSignal_Data
    {
        SlotData Slot;
        SignalData FinishedSignal;
    };

    void createAndAddSignal(QString signalName, ArgsList signalArgs = ArgsList());
    void createAndAddSlot(QString slotReturnType /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsList slotArgs = ArgsList());

    void createAndAddRequestResponse_aka_SlotWithFinishedSignal(QString slotName, ArgsList slotArgs = ArgsList(), ArgsList signalArgs_inAdditionToSuccessBooleanThatWillBeAutoAdded = ArgsList(), QString signalName_orLeaveEmptyToAutoGenerateFinishedSignalNameUsingSlotName = QString());

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

    static SignalData createSignal(QString signalName, ArgsList signalArgs);
    static SlotData createSlot(QString slotReturnType, QString slotName, ArgsList slotArgs);
};

#endif // USERINTERFACESKELETONGENERATORDATA_H
