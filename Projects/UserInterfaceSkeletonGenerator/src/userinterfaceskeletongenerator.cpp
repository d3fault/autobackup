#include "userinterfaceskeletongenerator.h"

#include <QTextStream>
#include <QTemporaryFile>
#include <QMetaObject>
#include <QDebug>

QString UserInterfaceSkeletonGenerator::TAB = "    ";

UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGenerator(QObject *parent)
    : QObject(parent)
{
    connect(this, &UserInterfaceSkeletonGenerator::e, this, &UserInterfaceSkeletonGenerator::handleDbg);
    connect(this, &UserInterfaceSkeletonGenerator::o, this, &UserInterfaceSkeletonGenerator::handleDbg);
}
void UserInterfaceSkeletonGenerator::displayFrontendBackendConnectStatements(const UserInterfaceSkeletonGeneratorData &data)
{
    //TODOmb: we could inject this shit directly into the class declaration (input to generateUserInterfaceSkeletonFromClassDeclarationString), but for now it's good enough to just print to screen and tell the user to copy/paste it :). much easier than using libclang xD
    if(data.Slots.isEmpty() /*&& data.Signals.isEmpty()*/)
        return; //TODOreq: we will have signals at some point too, so we need at least one of one or the other

    QString temp("copy/paste this method into the public area of your business logic class:\n\n");

    temp.append(TAB);
    temp.append("inline void connectToUi(");
    temp.append(data.targetUserInterfaceClassName() + "*ui)\n");
    temp.append(TAB + "{\n");
    Q_FOREACH(UserInterfaceSkeletonGeneratorData::SlotData slotData, data.Slots)
    {
        //Qt5 fail: temp.append(TAB + TAB + "connect(ui->asQObject(), &" + data.targetUserInterfaceClassName() + "::" + slotData.correspondingRequestSignalName() + ", this, &" + data.BusinessLogiClassName + "::" + slotData.SlotName + ");\n"); //TODOreq: use Qt4 string-based syntax, Qt5 style doesn't work here!
        temp.append(TAB + TAB + "connect(ui->asQObject(), SIGNAL(" + slotData.correspondingRequestSignalName() + slotData.argTypesNormalizedAndWithParenthesis() + "), this, SLOT(" + slotData.SlotName + slotData.argTypesNormalizedAndWithParenthesis() + "));\n");
    }
    temp.append(TAB + "}\n");

    temp.append("\n\n");

    emit o(temp);
}
void UserInterfaceSkeletonGenerator::generateUserInterfaceSkeletonFromClassDeclarationString()
{
    UserInterfaceSkeletonGeneratorData data; //TODOreq: data gets populated, with the help of libclang, from this class method's [not yet existing] QString classDeclaration
    data.BusinessLogiClassName = "LibFfmpeg";

    data.createAndAddSlot("bool", "encodeVideo", UserInterfaceSkeletonGeneratorData::ArgsList() << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","input"} << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","output"} << UserInterfaceSkeletonGeneratorData::SingleArg{"QString","fourCC"} ); //TODOoptimization: don't require those huge prefixes. since I'm going to be MODIFYING this code in order to USE the app [initially], it's a huge optimization xD. use a namespace or something (and do using namespace blah; at top of this file)

    data.createAndAddSlot("void", "fuck");

    generateUserInterfaceSkeletonFromData(data);
}
void UserInterfaceSkeletonGenerator::generateUserInterfaceSkeletonFromData(const UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData &data)
{
    displayFrontendBackendConnectStatements(data);

    QTemporaryFile file(data.targetUserInterfaceClassName().toLower() + "-XXXXXX.h");
    if(!file.open())
    {
        emit e("failed to open file: " + file.fileName());
        emit finishedGeneratingUserInterfaceSkeleton(false); //oh right, "finished" comes before corresponding slot name, not after ;-P
        return;
    }
    file.setAutoRemove(false);
    emit o("the output you desire is here: " + file.fileName());
    QTextStream textStream(&file);
    data.generateCpp(textStream);
    emit finishedGeneratingUserInterfaceSkeleton(true);
}
void UserInterfaceSkeletonGenerator::handleDbg(QString msg)
{
    qDebug() << msg; //TODOreq: proper
}
void UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::createAndAddSlot(QString slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType, QString slotName, ArgsList slotArgs)
{
    SlotData slotData;
    slotData.SlotReturnType = slotReturnTypeThatGetsConvertedToTheFinishedSignalsSingleArgType;
    slotData.SlotName = slotName;
    slotData.SlotArgs = slotArgs;
    Slots.append(slotData);
}
void UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::generateCpp(QTextStream &textStream) const
{
    textStream << "#include <QString>" << endl << endl; //TODOreq: auto-include based on arg types and slot return types... ffffff....

    textStream << "class QObject;" << endl << endl;

    textStream << "class " << targetUserInterfaceClassName() << endl << "{" << endl;

    textStream << "public:" << endl;
    textStream << TAB << targetUserInterfaceClassName() << "()=default;" << endl;
    textStream << TAB << targetUserInterfaceClassName() << "(const " << targetUserInterfaceClassName() << " &other)=delete;" << endl;
    textStream << TAB << "virtual ~" << targetUserInterfaceClassName() << "()=default;" << endl << endl;

    textStream << TAB << "virtual QObject *asQObject()=0;" << endl;

    if(!Slots.isEmpty())
        textStream << "protected: //signals:" << endl;
    Q_FOREACH(SlotData slotData, Slots)
    {
        textStream << TAB << "virtual void " /*always void because is signal! not to be confused with SlotReturnType, which comes back as an ARG of this signal*/ << slotData.correspondingRequestSignalName() + slotData.argsWithParenthesis() << "=0;" << endl;
        //TODOreq: gen a slot called "handleSlotNameFinished", and if slotData.SlotReturnType is non-void, an arg of that type (called something generic) should be it's only arg. maybe actually I should use bool always as first param (bool success), and the return type specified is an OPTIONAL 2nd arg following bool success? actually yea I like this idea better!!! TODOreq
    }

    textStream << "};" << endl;

    //TODOreq: header guards
}
QString UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::targetUserInterfaceClassName() const
{
    return QString("I" + BusinessLogiClassName + "UI"); //"UserInterface" suffix too verbose, esp since "I" is at beginning as well xDDDDD. even though I know the 'I' at beginning stands for interface, I tend to read it as "I am a Business Logic Class Name UI", the 'I' taking on a different meaning than "Interface" there (and it helps with udnerstandability imo)
}
QString UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::SlotData::argsWithoutParenthesis() const
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

QString UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::SlotData::argsWithParenthesis() const
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
QString UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::SlotData::argTypesNormalizedWithoutParenthesis() const
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
QString UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::SlotData::argTypesNormalizedAndWithParenthesis() const
{
    QString ret("(");
    ret.append(argTypesNormalizedWithoutParenthesis());
    ret.append(")");
    return ret;
}
QString UserInterfaceSkeletonGenerator::UserInterfaceSkeletonGeneratorData::SlotData::correspondingRequestSignalName() const
{
    return SlotName + "Requested";
}
