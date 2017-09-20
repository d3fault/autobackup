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

    struct SlotData
    {
        QString SlotReturnType;
        QString SlotName;
        ArgsList SlotArgs;

        QString argsWithoutParenthesis() const;
        QString argsWithParenthesis() const;
        QString argTypesNormalizedWithoutParenthesis() const;
        QString argTypesNormalizedAndWithParenthesis() const;

        QString correspondingRequestSignalName() const;
    };

    void createAndAddSlot(QString slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType/*TODOreq*/ /*ex: "void"*/, QString slotName /*ex: "encodeVideo*/, ArgsList slotArgs = ArgsList());

    void generatePureVirtualUserInterfaceHeaderFile(QTextStream &textStream) const;

    QString targetUserInterfaceClassName() const;

    QString BusinessLogiClassName;
    QList<SlotData> Slots;
};

#endif // USERINTERFACESKELETONGENERATORDATA_H
