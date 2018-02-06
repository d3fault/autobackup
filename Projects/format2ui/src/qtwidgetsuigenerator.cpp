#include "qtwidgetsuigenerator.h"

#include <QFile>
#include <QSet>
#include <QDebug>

#define QtWidgetsUiGenerator_SOURCE_FILEPATH "firstnamelastnameqtwidget.cpp"
#define QtWidgetsUiGenerator_HEADER_FILEPATH "firstnamelastnameqtwidget.h"

QStringList QtWidgetsUiGenerator::filesToGenerate() const
{
    return QStringList { QtWidgetsUiGenerator_SOURCE_FILEPATH, QtWidgetsUiGenerator_HEADER_FILEPATH };
}
bool QtWidgetsUiGenerator::generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const UIGeneratorFormat &format)
{
    if(theRelativeFilePathInWhichToGenerate == QtWidgetsUiGenerator_SOURCE_FILEPATH)
    {
        if(!generateSource(currentFileTextStream, format))
            return false;
    }
    else if(theRelativeFilePathInWhichToGenerate == QtWidgetsUiGenerator_HEADER_FILEPATH)
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
        streamOutUiVariableInstantiationCode(outputSourceFileTextStream, currentUiVariable);
    }
    qDebug() << "your output is here:" << outputDir.path();
    return true;
#endif
}
void QtWidgetsUiGenerator::replaceSpecialCommentSection(QString *out_Source, const QString &specialIdInTheSpecialComments, const QString &whatToReplaceItWith)
{
    int indexOfBegin = out_Source->indexOf("/*format2ui-compiling-template-example_BEGIN_" + specialIdInTheSpecialComments + "*/");
    QString endKey = "/*format2ui-compiling-template-example_END_" + specialIdInTheSpecialComments + "*/";
    int indexOfEnd = out_Source->indexOf(endKey);
    int indexAfterLastCharOfEnd = indexOfEnd + endKey.size();
    int len = indexAfterLastCharOfEnd-indexOfBegin;
    out_Source->replace(indexOfBegin, len, whatToReplaceItWith);
}
bool QtWidgetsUiGenerator::generateSource(QTextStream &currentFileTextStream, const UIGeneratorFormat &format)
{
    //read in source from compiling template example
    QFile compilingTemplateExampleSourceFile("/home/user/text/Projects/format2ui/design/src/QtWidgetsUiGeneratorOutputCompilingTemplateExample/src/qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.cpp");
    if(!compilingTemplateExampleSourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "failed to open file for reading:" << compilingTemplateExampleSourceFile.fileName();
        return false;
    }
    QTextStream compilingTemplateExampleSourceTextStream(&compilingTemplateExampleSourceFile);
    QString compilingTemplateExampleSource = compilingTemplateExampleSourceTextStream.readAll();

    //strReplace shiz on the file contents

    QString whatToLookFor0 = "    , m_FirstNameLineEdit(new QLineEdit())\n    , m_LastNameLineEdit(new QLineEdit())";
    QString whatToReplaceItWith0;

    QString whatToLookFor1("    m_FirstNameLineEdit->setPlaceholderText(\"First Name:\");\n    myLayout->addWidget(m_FirstNameLineEdit);\n    m_LastNameLineEdit->setPlaceholderText(\"Last Name:\");\n    myLayout->addWidget(m_LastNameLineEdit);");
    QString whatToReplaceItWith1;

    QString whatToLookFor2("    connect(m_FirstNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);\n    connect(m_LastNameLineEdit, &QLineEdit::returnPressed, okButton, &QPushButton::click);");
    QString whatToReplaceItWith2;

    QString whatToLookFor3 = "        if(!m_ArgParser.firstNameDefaultValueParsedFromProcessArg().isEmpty())\n            m_FirstNameLineEdit->setText(m_ArgParser.firstNameDefaultValueParsedFromProcessArg());\n        if(!m_ArgParser.lastNameDefaultValueParsedFromProcessArg().isEmpty())\n            m_LastNameLineEdit->setText(m_ArgParser.lastNameDefaultValueParsedFromProcessArg());";
    QString whatToReplaceItWith3;

    //QString whatToLookFor4 = "    QString firstName = m_FirstNameLineEdit->text();\n    if(firstName.isEmpty())\n    {\n        QMessageBox::critical(this, \"Error\", \"First Name cannot be empty\");\n        return;\n    }\n    QString lastName = m_LastNameLineEdit->text();\n    if(lastName.isEmpty())\n    {        QMessageBox::critical(this, \"Error\", \"Last Name cannot be empty\");\n        return;\n    }";
    QString whatToReplaceItWith4_ldkjsflj238423084;

    QString whatToLookFor5("firstName, lastName");
    QString whatToReplaceItWith5;

    bool first = true;
    for(QList<UIVariable>::const_iterator it = format.UIVariables.constBegin(); it != format.UIVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        switch(uiVariable.Type)
        {
            case UIVariableType::LineEdit_String:
                {
                    QString lineEditMemberName = lineEditMemberVariableName(uiVariable.VariableName);
                    whatToReplaceItWith0 += "    , " + lineEditMemberName + "(new QLineEdit())\n";
                    whatToReplaceItWith1 += "    " + lineEditMemberName + "->setPlaceholderText(\"" + uiVariable.HumanReadableNameForShowingFinalEndUser + ":\");\n    myLayout->addWidget(" + lineEditMemberName + ");\n";
                    whatToReplaceItWith2 += "    connect(" + lineEditMemberName + ", &QLineEdit::returnPressed, okButton, &QPushButton::click);\n";
                    whatToReplaceItWith3 += "        if(!m_ArgParser." + uiVariable.VariableName + "DefaultValueParsedFromProcessArg().isEmpty())\n            " + lineEditMemberName + "->setText(m_ArgParser." + uiVariable.VariableName + "DefaultValueParsedFromProcessArg());\n";
                    whatToReplaceItWith4_ldkjsflj238423084 += "    QString " + uiVariable.VariableName + " = " + lineEditMemberName + "->text();\n    if(" + uiVariable.VariableName + ".isEmpty())\n    {\n        QMessageBox::critical(this, \"Error\", \"" + uiVariable.HumanReadableNameForShowingFinalEndUser + " cannot be empty\");\n        return;\n    }\n";
                    whatToReplaceItWith5 += QString(first ? "" : ", ") + uiVariable.VariableName;
                }
                break;
            case UIVariableType::PlainTextEdit_StringList:
                {
                    //TODOreq:
                    QString plainTextEditMemberName = plainTextEditMemberVariableName(uiVariable.VariableName);
                    whatToReplaceItWith1 += "    myLayout->addWidget(new QLabel(\"" + uiVariable.HumanReadableNameForShowingFinalEndUser + "\"));\n    myLayout->addWidget(" + plainTextEditMemberName + ");\n";

                    whatToReplaceItWith4_ldkjsflj238423084 += "    QStringList " + uiVariable.VariableName + " = " + plainTextEditMemberName + "->toPlainText().split(\"\\n\");\n";
                }
                break;
            default:
                qWarning("unknown ui variable type");
                break;
        }
        //etc
        //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh

        first = false;
    }

    compilingTemplateExampleSource.replace(whatToLookFor0, whatToReplaceItWith0);
    compilingTemplateExampleSource.replace(whatToLookFor1, whatToReplaceItWith1);
    compilingTemplateExampleSource.replace(whatToLookFor2, whatToReplaceItWith2);
    compilingTemplateExampleSource.replace(whatToLookFor3, whatToReplaceItWith3);
    //compilingTemplateExampleSource.replace(whatToLookFor4, whatToReplaceItWith4_ldkjsflj238423084);
    replaceSpecialCommentSection(&compilingTemplateExampleSource, "ldkjsflj238423084" , whatToReplaceItWith4_ldkjsflj238423084);
    compilingTemplateExampleSource.replace(whatToLookFor5, whatToReplaceItWith5);


    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleSource;

    return true;
}
bool QtWidgetsUiGenerator::generateHeader(QTextStream &currentFileTextStream, const UIGeneratorFormat &format)
{
    //read in header from compiling template example
    QFile compilingTemplateExampleHeaderFile("/home/user/text/Projects/format2ui/design/src/QtWidgetsUiGeneratorOutputCompilingTemplateExample/src/qtwidgetsuigeneratoroutputcompilingtemplateexamplewidget.h");
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

    QString whatToLookFor2 = "    QLineEdit *m_FirstNameLineEdit;\n    QLineEdit *m_LastNameLineEdit;";
    QString whatToReplaceItWith2;

    QString whatToLookFor3("const QString &firstName, const QString &lastName");
    QString whatToReplaceItWith3;

    QString whatToLookFor4("class QLineEdit;");
    QSet<QString> whatToReplaceItWith4_forwardDeclareClasses;

    bool first = true;
    for(QList<UIVariable>::const_iterator it = format.UIVariables.constBegin(); it != format.UIVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        switch(uiVariable.Type)
        {
            case UIVariableType::LineEdit_String:
                {
                    whatToReplaceItWith4_forwardDeclareClasses.insert("QLineEdit");
                    QString lineEditMemberName = lineEditMemberVariableName(uiVariable.VariableName);
                    whatToReplaceItWith2 += "    QLineEdit *" + lineEditMemberName + ";\n";
                    whatToReplaceItWith3 += QString(first ? "" : ", ") + QString("const QString &") + uiVariable.VariableName;
                }
                break;
            case UIVariableType::PlainTextEdit_StringList:
                {
                    whatToReplaceItWith4_forwardDeclareClasses.insert("QPlainTextEdit");
                    whatToReplaceItWith2 += "    QPlainTextEdit *" + plainTextEditMemberVariableName(uiVariable.VariableName) + ";\n";
                }
                break;
                //etc
            default:
                qWarning("unknown uiVariable type");
                break;
        }

        first = false;
    }
    QString whatToReplaceItWith4;
    for(QSet<QString>::const_iterator it = whatToReplaceItWith4_forwardDeclareClasses.constBegin(); it != whatToReplaceItWith4_forwardDeclareClasses.constEnd(); ++it)
    {
        whatToReplaceItWith4 += "class " + (*it) + ";\n";
    }

    compilingTemplateExampleHeader.replace(whatToLookFor0, whatToReplaceItWith0);
    compilingTemplateExampleHeader.replace(whatToLookFor1, whatToReplaceItWith1);
    compilingTemplateExampleHeader.replace(whatToLookFor2, whatToReplaceItWith2);
    compilingTemplateExampleHeader.replace(whatToLookFor3, whatToReplaceItWith3);
    compilingTemplateExampleHeader.replace(whatToLookFor4, whatToReplaceItWith4);

    //write out to currentFileTextStream
    currentFileTextStream << compilingTemplateExampleHeader;

    return true;
}
QString QtWidgetsUiGenerator::lineEditMemberVariableName(const QString &variableName)
{
    QString ret = "m_" + firstLetterToUpper(variableName) + "LineEdit";
    return ret;
}
QString QtWidgetsUiGenerator::plainTextEditMemberVariableName(const QString &variableName)
{
    QString ret = "m_" + firstLetterToUpper(variableName) + "PlainTextEdit";
    return ret;
}
