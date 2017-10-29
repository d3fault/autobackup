#include "iuserinterfaceimplstubgenerator.h"

using namespace UserInterfaceSkeletonGeneratorData;

QString IUserInterfaceImplStubGenerator::headerGuardDefine(const QString &businessLogiClassName) const
{
    QString ret(targetImplStubClassName(businessLogiClassName).toUpper());
    ret.append("_H");
    return ret;
}
void IUserInterfaceImplStubGenerator::generateSignalsAndSlotsHeaderDeclarations(const Data &data, QTextStream &textStream)
{
    //gen 'requested' signals to go with business slots
    if(!data.Slots.isEmpty())
        textStream << "signals:" << endl;
    Q_FOREACH(SlotData currentSlot, data.Slots)
    {
        textStream << tab << "void " << currentSlot.correspondingSlotInvokeRequestSignalName() << currentSlot.argsWithParenthesis(currentSlot.slotArgs()) << ";" << endl;
    }

    //gen 'handle' slots to go with business signals
    if(!data.Signals.isEmpty())
        textStream << "public slots:" << endl;
    Q_FOREACH(SignalData currentSignal, data.Signals)
    {
        textStream << tab << "void " << currentSignal.correspondingSignalHandlerSlotName() << currentSignal.argsWithParenthesis(currentSignal.signalArgs()) << ";" << endl;
    }
}
void IUserInterfaceImplStubGenerator::generate_Q_OBJECT_inherittingClassHeader(const Data &data, QTextStream &textStream, QString directlyInherittedBaseClass)
{
    textStream << "#ifndef " << headerGuardDefine(data.BusinessLogicClassName) << endl;
    textStream << "#define " << headerGuardDefine(data.BusinessLogicClassName) << endl;

    textStream << endl;

    textStream << "#include <" << directlyInherittedBaseClass << ">" << endl;
    //textStream << "#include \"" << data.targetUserInterfaceClassName().toLower() << ".h" << endl;
    textStream << endl;

    textStream << "class " << targetImplStubClassName(data.BusinessLogicClassName) << " : public " << directlyInherittedBaseClass << endl;
    textStream << "{" << endl;

    textStream << tab << "Q_OBJECT" << endl;
    textStream << "public:" << endl;
    textStream << tab << "explicit " << targetImplStubClassName(data.BusinessLogicClassName) << "(" << directlyInherittedBaseClass << " *parent = nullptr);" << endl;
    //textStream << tab << "virtual ~" << targetImplStubClassName(data.BusinessLogicClassName) << "()=default;" << endl;

    //OT'ish: hmm sometimes it's desireable to connect a backend to a deeply nested front-end gui component (doesn't really apply to cli I don't think, but mb it does). is that now not possible? or would that deeply nested component use it's own interface that the top-level gui does not? hmm, wtf. I still think for common cases this could save some typing...

    generateSignalsAndSlotsHeaderDeclarations(data, textStream);

    textStream << "};" << endl;

    textStream << endl;

    textStream << "#endif // " << headerGuardDefine(data.BusinessLogicClassName) << endl;
}
void IUserInterfaceImplStubGenerator::generate_Q_OBJECT_inherittingStandardEmptyConstructorSourceCode(const Data &data, QTextStream &textStream, const QString &directlyInherittedBaseClass)
{
    textStream << "#include \"" << targetImplStubClassName(data.BusinessLogicClassName).toLower() << ".h\"" << endl << endl;

    textStream << targetImplStubClassName(data.BusinessLogicClassName) << "::" << targetImplStubClassName(data.BusinessLogicClassName) << "(" << directlyInherittedBaseClass << " *parent)" << endl;
    textStream << tab << ": " << directlyInherittedBaseClass << "(parent)" << endl;
    textStream << "{ }" << endl;
}
void IUserInterfaceImplStubGenerator::generateSignalHandlerSlotsSourceCode(const Data &data, QTextStream &textStream)
{
    Q_FOREACH(const SignalData &currentSignal, data.Signals)
    {
        QString qualifiedSlotNameAndArgs(targetImplStubClassName(data.BusinessLogicClassName) + "::" + currentSignal.correspondingSignalHandlerSlotName() + currentSignal.argsWithParenthesis(currentSignal.signalArgs()));
        textStream << "void " << qualifiedSlotNameAndArgs << endl;
        textStream << "{" << endl;
        textStream << tab << "//TODOstub" << endl;
        textStream << tab << "qWarning(\"stub not implemented: " << qualifiedSlotNameAndArgs << "\");" << endl;
        textStream << "}" << endl;
    }
}
