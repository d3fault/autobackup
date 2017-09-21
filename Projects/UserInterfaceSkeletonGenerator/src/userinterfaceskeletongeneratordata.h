#ifndef USERINTERFACESKELETONGENERATORDATA_H
#define USERINTERFACESKELETONGENERATORDATA_H

#include <QTextStream>

struct UserInterfaceSkeletonGeneratorData
{
    struct SingleArg
    {
        QString ArgType;
        QString ArgName;
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

    void createAndAddSignal(QString signalName, ArgsList signalArgs = ArgsList());
    void createAndAddSlot(QString slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType/*TODOreq*/ /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsList slotArgs = ArgsList());

    //void generatePureVirtualUserInterfaceHeaderFile(QTextStream &textStream) const;

    //QString targetUserInterfaceClassName() const;

    QString BusinessLogiClassName;
    QList<SignalData> Signals;
    QList<SlotData> Slots;
private:
    static QString firstLetterToUpper(const QString &inputString);
};

#endif // USERINTERFACESKELETONGENERATORDATA_H
