#include "qtcliuigenerator.h"

#include <QFile>
#include <QDebug>

#define QtCliUiGenerator_SOURCE_FILEPATH "firstnamelastnameqtcli.cpp"
#define QtCliUiGenerator_HEADER_FILEPATH "firstnamelastnameqtcli.h"

QStringList QtCliUiGenerator::filesToGenerate() const
{
    return QStringList { QtCliUiGenerator_SOURCE_FILEPATH, QtCliUiGenerator_HEADER_FILEPATH };
}
bool QtCliUiGenerator::generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UIGeneratorFormat &format)
{
    if(theRelativeFilePathInWhichToGenerate == QtCliUiGenerator_SOURCE_FILEPATH)
    {
        if(!generateSource(currentFileTextStream, format))
            return false;
    }
    else if(theRelativeFilePathInWhichToGenerate == QtCliUiGenerator_HEADER_FILEPATH)
    {
        if(!generateHeader(currentFileTextStream, format))
            return false;
    }
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
bool QtCliUiGenerator::generateSource(QTextStream &currentFileTextStream, const UIGeneratorFormat &format)
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

    QString whatToLookFor1 = "firstName, lastName";
    QString whatToReplaceItWith1;

    bool first = true;
    for(QList<UIVariable>::const_iterator it = format.UIVariables.constBegin(); it != format.UIVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        switch(uiVariable.Type)
        {
            case UIVariableType::LineEdit_String:
                {
                    whatToReplaceItWith0 += "    QString " + uiVariable.VariableName + " = query(\"" + uiVariable.HumanReadableNameForShowingFinalEndUser + "\", m_ArgParser." + uiVariable.VariableName + "DefaultValueParsedFromProcessArg());\n";
                    whatToReplaceItWith1 += (first ? "" : ", ") + uiVariable.VariableName;
                }
                break;
            case UIVariableType::PlainTextEdit_StringList:
                {
                    //TODOreq:
                }
                break;
            default:
                qWarning("unknown ui variable type");
                break;
        }
        //etc

        first = false;
    }

    compilingTemplateExampleSource.replace(whatToLookFor0, whatToReplaceItWith0);
    compilingTemplateExampleSource.replace(whatToLookFor1, whatToReplaceItWith1);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleSource;

    return true;
}
bool QtCliUiGenerator::generateHeader(QTextStream &currentFileTextStream, const UIGeneratorFormat &format)
{
    //read in header from compiling template example
    QFile compilingTemplateExampleHeaderFile("/home/user/text/Projects/format2ui/design/src/QtCliUiGeneratorOutputCompilingTemplateExample/src/qtcliuigeneratoroutputcompilingtemplateexample.h");
    if(!compilingTemplateExampleHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "failed to open file for reading:" << compilingTemplateExampleHeaderFile.fileName();
        return false;
    }
    QTextStream compilingTemplateExampleHeaderTextStream(&compilingTemplateExampleHeaderFile);
    QString compilingTemplateExampleHeader = compilingTemplateExampleHeaderTextStream.readAll();

    //strReplace shiz on the file contents

    QString commandLineParserObjectName = format.Name + "CommandLineOptionParser";

    QString whatToLookFor0 = "../../uishared/firstnamelastnameqobjectcommandlineoptionparser.h";
    QString whatToReplaceItWith0("../uishared/" + commandLineParserObjectName.toLower() + ".h");

    QString whatToLookFor1("FirstNameLastNameQObjectCommandLineOptionParser");
    QString whatToReplaceItWith1(commandLineParserObjectName);

    QString whatToLookFor2("const QString &firstName, const QString &lastName");
    QString whatToReplaceItWith2;

    bool first = true;
    for(QList<UIVariable>::const_iterator it = format.UIVariables.constBegin(); it != format.UIVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        switch(uiVariable.Type)
        {
            case UIVariableType::LineEdit_String:
                {
            whatToReplaceItWith2 += QString(first ? "" : ", ") + QString("const QString &") + uiVariable.VariableName;
                }
                break;
            case UIVariableType::PlainTextEdit_StringList:
                {
                    //TODOreq;
                }
                break;
            default:
                qWarning("unknown uiVariable type");
                break;
        }
        //etc

        first = false;
    }

    compilingTemplateExampleHeader.replace(whatToLookFor0, whatToReplaceItWith0);
    compilingTemplateExampleHeader.replace(whatToLookFor1, whatToReplaceItWith1);
    compilingTemplateExampleHeader.replace(whatToLookFor2, whatToReplaceItWith2);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleHeader;

    return true;
}
