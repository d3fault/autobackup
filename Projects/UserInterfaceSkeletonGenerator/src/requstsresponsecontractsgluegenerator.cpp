#include "requstsresponsecontractsgluegenerator.h"

#include <QFile>
#include <QTextStream>

QString RequstsResponseContractsGlueGenerator::TAB = "    ";

RequstsResponseContractsGlueGenerator::RequstsResponseContractsGlueGenerator(QObject *parent)
    : QObject(parent)
{ }
bool RequstsResponseContractsGlueGenerator::generateRequstsResponseContractsGlue_AndAddContractSignalsAndSlotsToData(UserInterfaceSkeletonGeneratorData *data, QString targetDir_WithTrailingSlash)
{
    //emit on error only (it used to be my finished signal), and only false

    addSignalsAndSlotsToData(data);

    const UserInterfaceSkeletonGeneratorData &constData = *data;
    if(!generateBusinessObjectRequestResponseContractsHeaderFile(constData, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectRequestResponseContractsSourceFile(constData, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectRequestResponseContractsPriFile(constData, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectSomeSlotRequestResponseHeaderFiles(constData, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectSomeSlotRequestResponseSourceFiles(constData, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectSomeSlotScopedResponderHeaderFiles(constData, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectSomeSlotScopedResponderSourceFiles(constData, targetDir_WithTrailingSlash))
        return false;

    return true;
}
void RequstsResponseContractsGlueGenerator::addSignalsAndSlotsToData(UserInterfaceSkeletonGeneratorData *data)
{
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentRequestResponse, data->RequestResponses_aka_SlotsWithFinishedSignals)
    {
        data->Signals.append(currentRequestResponse.FinishedSignal);
        data->Slots.append(currentRequestResponse.Slot);
    }
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectRequestResponseContractsHeaderFile(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    QFile businessObjectRequestResponseContractsHeaderFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + "requestresponsecontracts.h");
    if(!businessObjectRequestResponseContractsHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit e("failed to open file for writing: " + businessObjectRequestResponseContractsHeaderFile.fileName());
        emit error(false);
        return false;
    }
    QTextStream t(&businessObjectRequestResponseContractsHeaderFile);

    QString businessObjectRequestResponseContracts = data.BusinessLogicClassName + "RequestResponseContracts";
    QString headerGuard(businessObjectRequestResponseContracts.toUpper() + "_H");

    t << "#ifndef " << headerGuard << endl;
    t << "#define " << headerGuard << endl;
    t << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        //even though these includes aren't needed by this header, these includes are here to save the BusinessClass from having to #include them all manually
        t << "#include \"" << currentContract.slotScopedResponderTypeName().toLower() << ".h\"" << endl; //ex: #include "someslotscopedresponder.h"
    }
    t << endl;
    t << "class " << data.BusinessLogicClassName << ";" << endl;
    t << endl;
    t << "namespace " << businessObjectRequestResponseContracts << endl;
    t << "{" << endl;
    t << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        t << "class " << currentContract.slotRequestResponseTypeName() << ";" << endl; //ex: SomeSlotRequestResponse;
    }
    t << endl;
    t << "class Contracts" << endl;
    t << "{" << endl;
    t << "public:" << endl;
    t << TAB << "explicit Contracts(" << data.BusinessLogicClassName << " *" << firstLetterToLower(data.BusinessLogicClassName) << ");" << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        //ex: SomeSlotRequestResponse *someSlot() const;
        t << TAB << currentContract.slotRequestResponseTypeName() << " *" << currentContract.Slot.slotName() << "() const;" << endl;
    }
    t << "private:" << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        //ex: SomeSlotRequestResponse *m_SomeSlot;
        t << TAB << currentContract.slotRequestResponseTypeName() << " *m_" << firstLetterToUpper(currentContract.Slot.slotName()) << ";" << endl;
    }
    t << "};" << endl;
    t << endl;
    t << "}" << endl;
    t << endl;
    t << "#endif // " << headerGuard << endl;

    return true;
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectRequestResponseContractsSourceFile(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    QFile businessObjectRequestResponseContractsSourceFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + "requestresponsecontracts.cpp");
    if(!businessObjectRequestResponseContractsSourceFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit e("failed to open file for writing: " + businessObjectRequestResponseContractsSourceFile.fileName());
        emit error(false);
        return false;
    }
    QTextStream t(&businessObjectRequestResponseContractsSourceFile);

    QString businessObjectRequestResponseContracts = data.BusinessLogicClassName + "RequestResponseContracts";
    t << "#include \"" << businessObjectRequestResponseContracts.toLower() << ".h\"" << endl;
    t << endl;
    t << "#include \"" << data.BusinessLogicClassName.toLower() << ".h\"" << endl;
    t << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        //ex: #include "someslotrequestresponse.h"
        t << "#include \"" << currentContract.slotRequestResponseTypeName().toLower() << ".h\"" << endl;
    }
    t << endl;
    t << "using namespace " << businessObjectRequestResponseContracts << ";" << endl;
    t << endl;
    t << "Contracts::Contracts(" << data.BusinessLogicClassName << " *" << firstLetterToLower(data.BusinessLogicClassName) << ")" << endl;
    bool first = true;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        //ex: : m_SomeSlot(new SomeSlotRequestResponse(businessObject, businessObject))
        //ex: , m_AnotherSlot(new AnotherSlotRequestResponse(businessObject, businessObject))
        if(first)
        {
            t << TAB << ": ";
            first = false;
        }
        else
            t << TAB << ", ";

        t << "m_" << firstLetterToUpper(currentContract.Slot.slotName()) << "(new " << currentContract.slotRequestResponseTypeName() << "(" << firstLetterToLower(data.BusinessLogicClassName) << ", " << firstLetterToLower(data.BusinessLogicClassName) << "))" << endl;
    }
    t << "{ }" << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        /*ex:   SomeSlotRequestResponse *Contracts::someSlot() const
                {
                    return m_SomeSlot;
                }
        */

        t << currentContract.slotRequestResponseTypeName() << " *Contracts::" << currentContract.Slot.slotName() << "() const" << endl;
        t << "{" << endl;
        t << TAB << "return m_" << firstLetterToUpper(currentContract.Slot.slotName()) << ";" << endl;
        t << "}" << endl;
    }
    return true;
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectRequestResponseContractsPriFile(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    QFile businessObjectRequestResponseContractsPriFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + "requestresponsecontracts.pri");
    if(!businessObjectRequestResponseContractsPriFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        emit e("failed to open file for writing: " + businessObjectRequestResponseContractsPriFile.fileName());
        emit error(false);
        return false;
    }
    QTextStream t(&businessObjectRequestResponseContractsPriFile);

    //TODOoptimization: find out how to use fkn .pri variables and save the result of $$system(pwd) .. I think it's a system call, but it could be looking up the pwd property on some system object (could be BOTH!)
    t << "INCLUDEPATH *= $$system(pwd)" << endl;
    t << endl;
    t << "HEADERS *=      $$system(pwd)/" << data.BusinessLogicClassName.toLower() << "requestresponsecontracts.h \\" << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        //ex: t << TAB << "$$system(pwd)/someslotrequestresponse.h \\" << endl;
        //    t << TAB << "$$system(pwd)/someslotscopedresponder.h" << endl;

        t << TAB << "$$system(pwd)/" << currentContract.Slot.slotName().toLower() << "requestresponse.h \\" << endl;
        t << TAB << "$$system(pwd)/" << currentContract.Slot.slotName().toLower() << "scopedresponder.h" << endl;
    }
    t << endl;
    t << "SOURCES *=      $$system(pwd)/" << data.BusinessLogicClassName.toLower() << "requestresponsecontracts.cpp \\" << endl;
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        t << TAB << "$$system(pwd)/" << currentContract.Slot.slotName().toLower() << "requestresponse.cpp \\" << endl;
        t << TAB << "$$system(pwd)/" << currentContract.Slot.slotName().toLower() << "scopedresponder.cpp" << endl;
    }
    return true;
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectSomeSlotRequestResponseHeaderFiles(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        QString someSlotRequestResponseTypeName = firstLetterToUpper(currentContract.Slot.slotName()) + "RequestResponse";
        QFile businessObjectSlotRequestResponseHeaderFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + someSlotRequestResponseTypeName.toLower() + ".h");
        if(!businessObjectSlotRequestResponseHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            emit e("failed to open file for writing: " + businessObjectSlotRequestResponseHeaderFile.fileName());
            emit error(false);
            return false;
        }
        QTextStream t(&businessObjectSlotRequestResponseHeaderFile);

        QString headerGuard = someSlotRequestResponseTypeName.toUpper() + "_H";
        t << "#ifndef " << headerGuard << endl;
        t << "#define " << headerGuard << endl;
        t << endl;
        t << "#include <QObject>" << endl;
        t << endl;
        t << "class " << data.BusinessLogicClassName << ";" << endl;
        t << endl;
        t << "namespace " << data.BusinessLogicClassName << "RequestResponseContracts" << endl;
        t << "{" << endl;
        t << endl;
        t << "class " << someSlotRequestResponseTypeName << " : public QObject" << endl;
        t << "{" << endl;
        t << TAB << "Q_OBJECT" << endl;
        t << "public:" << endl;
        t << TAB << "explicit " << someSlotRequestResponseTypeName << "(" << data.BusinessLogicClassName << " *" << firstLetterToLower(data.BusinessLogicClassName) << ", QObject *parent = 0);" << endl;
        t << endl;
        Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SingleArg &currentArgInFinishedSignal, currentContract.FinishedSignal.signalArgs())
        {
            //ex: void setXIsEven(bool xIsEven);
            t << TAB << "void set" << firstLetterToUpper(currentArgInFinishedSignal.argName()) << "(" << currentArgInFinishedSignal.ArgType << " " << currentArgInFinishedSignal.argName() << ");" << endl;
        }
        t << endl;
        t << TAB << "void reset();" << endl;
        t << "private:" << endl;
        t << TAB << "void respond_aka_emitFinishedSignal();" << endl;
        t << TAB << "friend class " << firstLetterToUpper(currentContract.Slot.slotName()) + "ScopedResponder;" << endl;
        t << endl;
        Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SingleArg &currentArgInFinishedSignal, currentContract.FinishedSignal.signalArgs())
        {
            //ex: bool m_XIsEven;
            t << TAB << currentArgInFinishedSignal.ArgType << " " << "m_" << firstLetterToUpper(currentArgInFinishedSignal.argName()) << ";" << endl;
        }
        t << "signals: /*private:*/" << endl;

        //ex: void someSlotFinished(bool success, bool xIsEven);
        t << TAB << "void " << currentContract.FinishedSignal.signalName() << "(";
        bool first = true;
        Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SingleArg &currentArgInFinishedSignal, currentContract.FinishedSignal.signalArgs())
        {
            if(!first)
            {
                t << ", ";
            }
            first = false;
            t << currentArgInFinishedSignal.ArgType << " " << currentArgInFinishedSignal.argName();
        }
        t << ");" << endl;

        t << "};" << endl;
        t << endl;
        t << "}" << endl;
        t << endl;
        t << "#endif // " << headerGuard << endl;
    }
    return true;
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectSomeSlotRequestResponseSourceFiles(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        QString someSlotRequestResponseTypeName = firstLetterToUpper(currentContract.Slot.slotName()) + "RequestResponse";
        QFile businessObjectSlotRequestResponseHeaderFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + someSlotRequestResponseTypeName.toLower() + ".cpp");
        if(!businessObjectSlotRequestResponseHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            emit e("failed to open file for writing: " + businessObjectSlotRequestResponseHeaderFile.fileName());
            emit error(false);
            return false;
        }
        QTextStream t(&businessObjectSlotRequestResponseHeaderFile);

        t << "#include \"" << someSlotRequestResponseTypeName.toLower() << ".h\"" << endl;
        t << endl;
        t << "#include \"" << data.BusinessLogicClassName.toLower() << ".h\"" << endl;
        t << endl;
        t << "using namespace " << data.BusinessLogicClassName << "RequestResponseContracts;" << endl;
        t << endl;
        t << someSlotRequestResponseTypeName << "::" << someSlotRequestResponseTypeName << "(" << data.BusinessLogicClassName << " *" << firstLetterToLower(data.BusinessLogicClassName) << ", QObject *parent)" << endl;
        t << TAB << ": QObject(parent)" << endl;
        //t << TAB << ", m_Success(false)" << endl; //maybe I'll init default values here someday, but until then I definitely at least want to init success to false
        t << "{" << endl;
        t << TAB << "reset();" << endl;
        t << TAB << "connect(this, &" << someSlotRequestResponseTypeName << "::" << currentContract.FinishedSignal.signalName() << ", " << firstLetterToLower(data.BusinessLogicClassName) << ", &" << data.BusinessLogicClassName << "::" << currentContract.FinishedSignal.signalName() << ");" << endl;
        t << "}" << endl;
        Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SingleArg &currentArg, currentContract.FinishedSignal.signalArgs())
        {
            //ex:   void SomeSlotRequestResponse::setSuccess(bool success)
            //      {
            //          m_Success = success;
            //      }

            t << "void " << someSlotRequestResponseTypeName << "::set" << firstLetterToUpper(currentArg.argName()) << "(" << currentArg.ArgType << " " << currentArg.argName() << ")" << endl;
            t << "{" << endl;
            t << TAB << "m_" << firstLetterToUpper(currentArg.argName()) << " = " << currentArg.argName() << ";" << endl;
            t << "}" << endl;
        }
        t << "void " << someSlotRequestResponseTypeName << "::reset()" << endl;
        t << "{" << endl;
        Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SingleArgWithOptionalDefaultValue &currentArg, currentContract.FinishedSignal.signalArgs())
        {
            if(currentArg.ArgOptionalDefaultValue.isNull())
                continue;
            t << TAB << "m_" << firstLetterToUpper(currentArg.argName(UserInterfaceSkeletonGeneratorData::SingleArg::DisplaySimpleAssignmentStyleInitialization)) << ";" << endl;
        }
        t << "}" << endl;
        t << "void " << someSlotRequestResponseTypeName << "::respond_aka_emitFinishedSignal()" << endl;
        t << "{" << endl;
        t << TAB << "emit " << currentContract.FinishedSignal.signalName() << "(";
        bool first = true;
        Q_FOREACH(const UserInterfaceSkeletonGeneratorData::SingleArg &currentArg, currentContract.FinishedSignal.signalArgs())
        {
            //ex: m_Success, m_XIsEven

            if(!first)
            {
                t << ", ";
            }
            first = false;
            t << "m_" << firstLetterToUpper(currentArg.argName());
        }
        t << ");" << endl;
        //t << TAB << "m_Success = false; //after emit we set back to false for the NEXT slot  invocation" << endl; //reset response arg back to default values maybe, but idk tbh and definitely not in this version
        t << TAB << "reset();" << endl;
        t << "}" << endl;
    }
    return true;
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectSomeSlotScopedResponderHeaderFiles(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        QString someSlotScopedResponderTypeName = firstLetterToUpper(currentContract.Slot.slotName()) + "ScopedResponder";
        QFile businessObjectSlotRequestResponseHeaderFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + someSlotScopedResponderTypeName.toLower() + ".h");
        if(!businessObjectSlotRequestResponseHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            emit e("failed to open file for writing: " + businessObjectSlotRequestResponseHeaderFile.fileName());
            emit error(false);
            return false;
        }
        QTextStream t(&businessObjectSlotRequestResponseHeaderFile);

        QString headerGuard = someSlotScopedResponderTypeName.toUpper() + "_H";
        t << "#ifndef " << headerGuard << endl;
        t << "#define " << headerGuard << endl;
        t << endl;
        t << "#include \"" << someSlotScopedResponderTypeName.toLower() << ".h" << endl;
        t << endl;
        t << "namespace " << data.BusinessLogicClassName << data.BusinessLogicClassName << "RequestResponseContracts" << endl;
        t << "{" << endl;
        t << endl;
        t << "class " << someSlotScopedResponderTypeName << endl;
        t << "{" << endl;
        t << "public:" << endl;
        t << TAB << someSlotScopedResponderTypeName << "(" << firstLetterToUpper(currentContract.Slot.slotName()) << "RequestResponse *response);" << endl;
        t << TAB << firstLetterToUpper(currentContract.Slot.slotName()) << "RequestResponse *response();" << endl;
        t << TAB << "void deferResponding();" << endl;
        t << TAB << "void resumeAutoRespondingDuringScopedDestruction();" << endl;
        t << TAB << "~" << someSlotScopedResponderTypeName << "();" << endl;
        t << "private:" << endl;
        t << TAB <<  firstLetterToUpper(currentContract.Slot.slotName()) << "RequestResponse *m_" << firstLetterToUpper(currentContract.Slot.slotName()) << "RequestResponse;" << endl;
        t << TAB << "bool m_DeferResponding;" << endl;
        t << "};" << endl;
        t << endl;
        t << "}" << endl;
        t << endl;
        t << "#endif // " << headerGuard << endl;
    }
    return true;
}
bool RequstsResponseContractsGlueGenerator::generateBusinessObjectSomeSlotScopedResponderSourceFiles(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    Q_FOREACH(const UserInterfaceSkeletonGeneratorData::RequestResponse_aka_SlotWithFinishedSignal_Data &currentContract, data.RequestResponses_aka_SlotsWithFinishedSignals)
    {
        QString someSlotScopedResponderTypeName = firstLetterToUpper(currentContract.Slot.slotName()) + "ScopedResponder";
        QFile businessObjectSlotRequestResponseHeaderFile(targetDir_WithTrailingSlash + data.BusinessLogicClassName.toLower() + someSlotScopedResponderTypeName.toLower() + ".cpp");
        if(!businessObjectSlotRequestResponseHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            emit e("failed to open file for writing: " + businessObjectSlotRequestResponseHeaderFile.fileName());
            emit error(false);
            return false;
        }
        QTextStream t(&businessObjectSlotRequestResponseHeaderFile);

        t << "#include \"" << someSlotScopedResponderTypeName.toLower() << ".h" << endl;
        t << endl;
        t << "using namespace " << data.BusinessLogicClassName << "RequestResponseContracts;" << endl;
        t << endl;
        QString someSlotRequestResponseTypeName = firstLetterToUpper(currentContract.Slot.slotName());
        someSlotRequestResponseTypeName.append("RequestResponse");
        t << someSlotScopedResponderTypeName << "::" << someSlotScopedResponderTypeName << "(" << someSlotRequestResponseTypeName << " *" << firstLetterToLower(someSlotRequestResponseTypeName) << ")" << endl;
        t << TAB << ": m_" << someSlotRequestResponseTypeName << "(" << firstLetterToLower(someSlotRequestResponseTypeName) << ")" << endl;
        t << TAB << ", m_DeferResponding(false)" << endl;
        t << "{ }" << endl;
        t << someSlotRequestResponseTypeName << " *" << someSlotScopedResponderTypeName << "::response()" << endl;
        t << "{" << endl;
        t << TAB << "return m_" << someSlotRequestResponseTypeName << ";" << endl;
        t << "}" << endl;
        t << "void " << someSlotScopedResponderTypeName << "::deferResponding()" << endl;
        t << "{" << endl;
        t << TAB << "m_DeferResponding = true;" << endl;
        t << "}" << endl;
        t << "void " << someSlotScopedResponderTypeName << "::resumeAutoRespondingDuringScopedDestruction()" << endl;
        t << "{" << endl;
        t << TAB << "m_DeferResponding = false;" << endl;
        t << "}" << endl;
        t << someSlotScopedResponderTypeName << "::~" << someSlotScopedResponderTypeName << "()" << endl;
        t << "{" << endl;
        t << TAB << "if(!m_DeferResponding)" << endl;
        t << TAB << TAB << "m_" << someSlotRequestResponseTypeName << "->respond_aka_emitFinishedSignal();" << endl;
        t << "}" << endl;
    }
    return true;
}
