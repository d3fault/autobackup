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
        generateSource(currentFileTextStream, uiVariables);
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
void QtCliUiGenerator::generateSource(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{
    for(QList<UIVariable>::const_iterator it = uiVariables.constBegin(); it != uiVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        if(uiVariable.Type == UIVariableType::LineEdit_String)
        {
            //TODOreq: this is old:
            currentFileTextStream << TAB_format2ui << "m_StdOut << \"" << uiVariable.HumanReadableNameForShowingFinalEndUser << ": \";" << endl;
            currentFileTextStream << TAB_format2ui << "cin >> " << uiVariable.VariableName << ";" << endl; //cin sucks but it demonstrates the intent here...
        }
        //etc
    }
}
void QtCliUiGenerator::generateHeader(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{

}
