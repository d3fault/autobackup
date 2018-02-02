#include "qtcliuigenerator.h"

#include <QTemporaryDir>
#include <QFile>
#include <QDebug>

bool QtCliUiGenerator::generateUi(const QJsonObject &jsonUiFormatInput)
{
    QList<UIVariable> uiVariables;
    populateUiVariables(jsonUiFormatInput, &uiVariables);

    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QString outputPathWithSlashAppended = appendSlashIfNeeded(outputDir.path());

    QFile outputHeaderFile(outputPathWithSlashAppended + "SomeFuckingCliYo.h"); //TODOreq: parse the "ui name" from the inputFormat json shiz
    if(!outputHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "failed to open file:" << outputHeaderFile.fileName();
        return false;
    }
    QTextStream outputHeaderFileTextStream(&outputHeaderFile);

    QFile outputSourceFile(outputPathWithSlashAppended + "SomeFuckingCliYo.cpp"); //TODOreq: parse the "ui name" from the inputFormat json shiz
    if(!outputSourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "failed to open file:" << outputSourceFile.fileName();
        return false;
    }
    QTextStream outputSourceFileTextStream(&outputSourceFile);

    for(QList<UIVariable>::const_iterator it = uiVariables.constBegin(); it != uiVariables.constEnd(); ++it)
    {
        const UIVariable &currentUiVariable = (*it);
        streamOutUiCliQueriesViaCinCoutShiz(outputSourceFileTextStream, currentUiVariable);
    }
    qDebug() << "your output is here:" << outputDir.path();
    return true;

    return true;
}
void QtCliUiGenerator::streamOutUiCliQueriesViaCinCoutShiz(QTextStream &outputSourceFileTextStream, const UIVariable &uiVariable)
{
    if(uiVariable.Type == UIVariableType::LineEdit_String)
    {
        outputSourceFileTextStream << TAB_format2ui << "m_StdOut << \"" << uiVariable.HumanReadableNameForShowingFinalEndUser << ": \";" << endl;
        outputSourceFileTextStream << TAB_format2ui << "cin >> " << uiVariable.VariableName << ";" << endl; //cin sucks but it demonstrates the intent here...
    }
    //etc
}
