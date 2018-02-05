#include "qtwidgetsuigenerator.h"

#include <QFile>
#include <QDebug>

#define QtWidgetsUiGenerator_SOURCE_FILEPATH "firstnamelastnameqtwidget.cpp"
#define QtWidgetsUiGenerator_HEADER_FILEPATH "firstnamelastnameqtwidget.h"

QStringList QtWidgetsUiGenerator::filesToGenerate() const
{
    return QStringList { QtWidgetsUiGenerator_SOURCE_FILEPATH, QtWidgetsUiGenerator_HEADER_FILEPATH };
}
bool QtWidgetsUiGenerator::generateUiForFile(const QString &theRelativeFilePathInWhichToGenerate, QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{
    if(theRelativeFilePathInWhichToGenerate == QtWidgetsUiGenerator_SOURCE_FILEPATH)
        generateSource(currentFileTextStream, uiVariables);
    else if(theRelativeFilePathInWhichToGenerate == QtWidgetsUiGenerator_SOURCE_FILEPATH)
        generateHeader(currentFileTextStream, uiVariables);
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
void QtWidgetsUiGenerator::generateSource(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{
    for(QList<UIVariable>::const_iterator it = uiVariables.constBegin(); it != uiVariables.constEnd(); ++it)
    {
        const UIVariable &uiVariable = *it;
        if(uiVariable.Type == UIVariableType::LineEdit_String)
        {
            //TODOreq: this is old:
            QString lineEditVariableName = uiVariable.VariableName + "LineEdit";
            currentFileTextStream << TAB_format2ui << "QLineEdit *" << lineEditVariableName << " = new QLineEdit();" << endl;
            currentFileTextStream << TAB_format2ui << lineEditVariableName << "->setPlaceholderText(\"" << uiVariable.HumanReadableNameForShowingFinalEndUser << "\");" << endl; //a QLabel off to the left would be better (use buddying, too!)
            currentFileTextStream << TAB_format2ui << "myLayout.addWidget(" << lineEditVariableName << ");" << endl;
            //TODOmb: connect to the line edit's signals? or do we wait until "ok" is pressed before we get the text results? either way works tbh. KISS [but do it right (aren't these contradictions?)]
        }
        //etc
        //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh
    }
}
void QtWidgetsUiGenerator::generateHeader(QTextStream &currentFileTextStream, const QList<UIVariable> &uiVariables)
{

}
