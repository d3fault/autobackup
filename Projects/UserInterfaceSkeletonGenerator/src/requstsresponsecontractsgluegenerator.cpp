#include "requstsresponsecontractsgluegenerator.h"

#include <QFile>
#include <QTextStream>

QString RequstsResponseContractsGlueGenerator::TAB = "    ";

RequstsResponseContractsGlueGenerator::RequstsResponseContractsGlueGenerator(QObject *parent)
    : QObject(parent)
{ }
bool RequstsResponseContractsGlueGenerator::generateRequstsResponseContractsGlue(const UserInterfaceSkeletonGeneratorData &data, QString targetDir_WithTrailingSlash)
{
    //emit on error only (it used to be my finished signal), and only false

    if(!generateBusinessObjectRequestResponseContractsHeaderFile(data, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectRequestResponseContractsSourceFile(data, targetDir_WithTrailingSlash))
        return false;
    if(!generateBusinessObjectRequestResponseContractsPriFile(data, targetDir_WithTrailingSlash))
        return false;

    return true;
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
