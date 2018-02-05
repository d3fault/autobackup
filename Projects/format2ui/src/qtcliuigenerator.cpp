#include "qtcliuigenerator.h"

#include <QFile>
#include <QDebug>

#define QtCliUiGenerator_SOURCE_FILEPATH "firstnamelastnameqtcli.cpp"
#define QtCliUiGenerator_HEADER_FILEPATH "firstnamelastnameqtcli.h"

QStringList QtCliUiGenerator::filesToGenerate() const
{
    return QStringList { QtCliUiGenerator_SOURCE_FILEPATH, QtCliUiGenerator_HEADER_FILEPATH };
}
bool QtCliUiGenerator::generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{
    if(theRelativeFilePathInWhichToGenerate == QtCliUiGenerator_SOURCE_FILEPATH)
    {
        if(!generateSource(currentFileTextStream, uiVariables))
            return false;
    }
    else if(theRelativeFilePathInWhichToGenerate == QtCliUiGenerator_HEADER_FILEPATH)
        generateHeader(currentFileTextStream, uiVariables);
    //etc

    return true;
#if 0
    for(QList<UIVariable>::const_iterator it = uiVariables.constBegin(); it != uiVariables.constEnd(); ++it)
    {
        const UIVariable &currentUiVariable = (*it);
        streamOutUiCliQueriesViaCinCoutShiz(outputSourceFileTextStream, currentUiVariable);
    }
    qDebug() << "your output is here:" << outputDir.path();
    return true;
#endif
}
bool QtCliUiGenerator::generateSource(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{
    //isn't it better (faster, ezier, smarter) to generate code by copy/pasting the "compiling template example" and then strReplacing the fuck out of THAT? I mean it genuinely sounds like it'd be LESS work (and changes to the compiling template example needn't NECESSARILY require any changes to format2ui (although often times, changes _are_ necessary (more strReplace ez shit)). in any case, I'm surely going to experiment with the file-copy->strReplace strategy :-D. I might learn it sucks, but I'll learn something in any case!
    //TODOreq: the compiling template example(s) would then become project resources (.qrc) to be shipped/deployed inside the binary! neat

    //read in source from compiling template example
    QFile compilingTemplateExampleSourceFile("/home/user/text/Projects/format2ui/design/src/QtCliUiGeneratorOutputCompilingTemplateExample/src/qtcliuigeneratoroutputcompilingtemplateexample.cpp");
    if(!compilingTemplateExampleSourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "failed to open file for reading:" << compilingTemplateExampleSourceFile.fileName();
        return false;
    }
    QTextStream compilingTemplateExampleSourceTextStream(&compilingTemplateExampleSourceFile);
    QString compilingTemplateExampleSource = compilingTemplateExampleSourceTextStream.readAll();

    //strReplace shiz on the file contents

    QString whatToLookFor0 = "    QString firstName = query(\"First Name\", m_ArgParser.firstNameDefaultValueParsedFromProcessArg());\n    QString lastName = query(\"Last Name\", m_ArgParser.lastNameDefaultValueParsedFromProcessArg());";
    QString whatToReplaceItWith0;

    QString whatToLookFor1 = "emit collectUiVariablesFinished(firstName, lastName);";
    QString whatToReplaceItWith1("emit collectUiVariablesFinished(");

    bool first = true;
    for(QList<UIVariable>::const_iterator it = uiVariables.constBegin(); it != uiVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        if(uiVariable.Type == UIVariableType::LineEdit_String)
        {
            whatToReplaceItWith0 += "    QString " + uiVariable.VariableName + " = query(\"" + uiVariable.HumanReadableNameForShowingFinalEndUser + "\", m_ArgParser." + uiVariable.VariableName + "DefaultValueParsedFromProcessArg());\n";
            whatToReplaceItWith1 += (first ? "" : ", ") + uiVariable.VariableName;
        }
        //etc

        first = false;
    }
    whatToReplaceItWith1.append(");");

    compilingTemplateExampleSource.replace(whatToLookFor0, whatToReplaceItWith0);
    compilingTemplateExampleSource.replace(whatToLookFor1, whatToReplaceItWith1);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleSource;

    return true;
}
void QtCliUiGenerator::generateHeader(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{

}
