#include "userinterfaceskeletongeneratordata.h"

#include "userinterfaceskeletongenerator.h"

void UserInterfaceSkeletonGeneratorData::createAndAddSlot(QString slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType, QString slotName, ArgsList slotArgs)
{
    SlotData slotData;
    slotData.SlotReturnType = slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType;
    slotData.SlotName = slotName;
    slotData.SlotArgs = slotArgs;
    Slots.append(slotData);
}
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
QString UserInterfaceSkeletonGeneratorData::SlotData::argsWithoutParenthesis() const
{
    QString ret;
    bool first = true;
    Q_FOREACH(SingleArg currentArg, SlotArgs)
    {
        if(!first)
            ret.append(", ");
        first = false;

        ret.append(currentArg.ArgType + " " + currentArg.ArgName); //TODOmb: if argType ends with "reference amp" or ptr asterisk, don't use a space. steal this code from d=i pls
    }
    return ret;
}

QString UserInterfaceSkeletonGeneratorData::SlotData::argsWithParenthesis() const
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
QString UserInterfaceSkeletonGeneratorData::SlotData::argTypesNormalizedWithoutParenthesis() const
{
    QString ret;
    bool first = true;
    Q_FOREACH(SingleArg currentArg, SlotArgs)
    {
        if(!first)
            ret.append(",");
        first = false;

        ret.append(myNormalizedType(currentArg.ArgType));
    }
    return ret;
}
QString UserInterfaceSkeletonGeneratorData::SlotData::argTypesNormalizedAndWithParenthesis() const
{
    QString ret("(");
    ret.append(argTypesNormalizedWithoutParenthesis());
    ret.append(")");
    return ret;
}
QString UserInterfaceSkeletonGeneratorData::SlotData::correspondingRequestSignalName() const
{
    return SlotName + "Requested";
}
