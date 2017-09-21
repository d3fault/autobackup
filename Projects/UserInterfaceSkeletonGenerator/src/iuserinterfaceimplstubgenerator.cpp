#include "iuserinterfaceimplstubgenerator.h"

void IUserInterfaceImplStubGenerator::generateSignalsAndSlotsHeaderDeclarations(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream)
{
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
}
void IUserInterfaceImplStubGenerator::generate_Q_OBJECT_inherittingClassHeader(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream, QString directlyInherittedBaseClass)
{
    textStream << "#include <" << directlyInherittedBaseClass << ">" << endl;
    //textStream << "#include \"" << data.targetUserInterfaceClassName().toLower() << ".h" << endl;
    textStream << endl;

    textStream << "class " << targetImplStubClassName(data.BusinessLogiClassName) << " : public " << directlyInherittedBaseClass << endl;
    textStream << "{" << endl;

    textStream << tab << "Q_OBJECT" << endl;
    textStream << "public:" << endl;
    textStream << tab << "explicit " << targetImplStubClassName(data.BusinessLogiClassName) << "(" << directlyInherittedBaseClass << " *parent = nullptr);" << endl;
    //textStream << tab << "virtual ~" << targetImplStubClassName(data.BusinessLogiClassName) << "()=default;" << endl;

    //OT'ish: hmm sometimes it's desireable to connect a backend to a deeply nested front-end gui component (doesn't really apply to cli I don't think, but mb it does). is that now not possible? or would that deeply nested component use it's own interface that the top-level gui does not? hmm, wtf. I still think for common cases this could save some typing...

    generateSignalsAndSlotsHeaderDeclarations(data, textStream);

    textStream << "};" << endl;
}
void IUserInterfaceImplStubGenerator::generate_Q_OBJECT_inherittingStandardEmptyConstructorSourceCode(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream, const QString &directlyInherittedBaseClass)
{
    textStream << "#include \"" << targetImplStubClassName(data.BusinessLogiClassName).toLower() << ".h\"" << endl << endl;

    textStream << targetImplStubClassName(data.BusinessLogiClassName) << "::" << targetImplStubClassName(data.BusinessLogiClassName) << "(" << directlyInherittedBaseClass << " *parent)" << endl;
    textStream << tab << ": " << directlyInherittedBaseClass << "(parent)" << endl;
    textStream << "{ }" << endl;
}
void IUserInterfaceImplStubGenerator::generateSignalHandlerSlotsSourceCode(const UserInterfaceSkeletonGeneratorData &data, QTextStream &textStream)
{
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
