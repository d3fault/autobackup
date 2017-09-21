#include "cliimplstubgenerator.h"

#include <QFile>
#include <QTextStream>

#include "userinterfaceskeletongenerator.h"

void CliImplStubGenerator::generateImplStubFiles(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    generateHeaderFile(data, outputDirWithTrailingSlash);
    generateSourceFile(data, outputDirWithTrailingSlash);
    //TODOmb: generate main.cpp? it's small so why not <3?
}
void CliImplStubGenerator::generateHeaderFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + targetImplStubClassName(data.BusinessLogiClassName).toLower() + ".h");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in CliImplStubGenerator");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);
    //data.gen-
    //so I guess the only reason I can't use "tell, don't ask" is because I'm implementing an interface and yea. well I mean I still could, but it wouldn't be worth it. would be too confusing

    textStream << "#include <QObject>" << endl;
    //textStream << "#include \"" << data.targetUserInterfaceClassName().toLower() << ".h" << endl << endl;

    textStream << "class " << targetImplStubClassName(data.BusinessLogiClassName) << " : public QObject" << endl; //this class will have a lot in common with the gui stub generator, but the gui one will inherit QWidget instead of QObject
    textStream << "{" << endl;

    QString tab = UserInterfaceSkeletonGenerator::TAB;
    textStream << tab << "Q_OBJECT" << endl;
    textStream << "public:" << endl;
    textStream << tab << "explicit " << targetImplStubClassName(data.BusinessLogiClassName) << "(QObject *parent = nullptr);" << endl;
    //textStream << tab << "virtual ~" << targetImplStubClassName(data.BusinessLogiClassName) << "()=default;" << endl;

    //OT'ish: hmm sometimes it's desireable to connect a backend to a deeply nested front-end gui component (doesn't really apply to cli I don't think, but mb it does). is that now not possible? or would that deeply nested component use it's own interface that the top-level gui does not? hmm, wtf. I still think for common cases this could save some typing...

    //gen 'requested' signals to go with business slots
    if(!data.Slots.isEmpty())
        textStream << "signals:" << endl;
    Q_FOREACH(UserInterfaceSkeletonGeneratorData::SlotData currentSlot, data.Slots)
    {
        textStream << tab << "void " << currentSlot.correspondingSlotInvokeRequestSignalName() << currentSlot.argsWithParenthesis() << ";" << endl;
    }

    //gen 'handle' slots to go with business signals
    if(!data.Signals.isEmpty())
        textStream << "public slots:" << endl;
    Q_FOREACH(UserInterfaceSkeletonGeneratorData::SignalData currentSignal, data.Signals)
    {
        textStream << tab << "void " << currentSignal.correspondingSignalHandlerSlotName() << currentSignal.argsWithParenthesis() << ";" << endl;
    }

    textStream << "};" << endl;
}
void CliImplStubGenerator::generateSourceFile(const UserInterfaceSkeletonGeneratorData &data, const QString &outputDirWithTrailingSlash)
{
    QFile file(outputDirWithTrailingSlash + targetImplStubClassName(data.BusinessLogiClassName).toLower() + ".cpp");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("failed to open file in CliImplStubGenerator");

        //emit finishedGeneratingUserInterfaceSkeleton(false);
        return; //TODOmb: return _false_ and don't continue?
    }
    QTextStream textStream(&file);

    QString tab = UserInterfaceSkeletonGenerator::TAB;
    textStream << "#include \"" << targetImplStubClassName(data.BusinessLogiClassName).toLower() << ".h\"" << endl << endl;

    textStream << targetImplStubClassName(data.BusinessLogiClassName) << "::" << targetImplStubClassName(data.BusinessLogiClassName) << "(QObject *parent)" << endl;
    textStream << tab << ": QObject(parent)" << endl;
    textStream << "{ }" << endl;

    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SignalData &currentSignal, data.Signals)
    {
        QString qualifiedSlotNameAndArgs(targetImplStubClassName(data.BusinessLogiClassName) + "::" + currentSignal.correspondingSignalHandlerSlotName() + currentSignal.argsWithParenthesis());
        textStream << "void " << qualifiedSlotNameAndArgs << endl;
        textStream << "{" << endl;
        textStream << tab << "//TODOstub" << endl;
        textStream << tab << "qWarning(\"stub not implemented: " << qualifiedSlotNameAndArgs << "\");" << endl;
        textStream << "}" << endl;
    }
}
QString CliImplStubGenerator::implStubClassSuffix()
{
    return "Cli";
}
