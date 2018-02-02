#include "qtwidgetsuigenerator.h"

#include <QTemporaryDir>
#include <QFile>
#include <QJsonArray>
#include <QDebug>

#define TAB_QtWidgetsUiGenerator "    "

bool QtWidgetsUiGenerator::generateUi(const QJsonObject &jsonUiFormatInput)
{
    //a ui format input is a series of layouts (containing "widgets" therein)
    //so as not to digress too fucking much, I'll start with a QVBoxLayout as my "root" layout. I use QVBoxLayout the most
    const QJsonObject &rootLayout = jsonUiFormatInput.value("rootLayout").toObject();
    //I could have returned an array, but dealing with objects is ezier imo -- (ed note: we return an array but later actually)

    //... something comes here...

    //eventually we end up with a list of {HumanReadableNameForShowingFinalEndUser,VariableName,VariableValue}
    //since I know the end result but not what comes in between (yet), I'll code backwards to it!

    QList<UIVariable> uiVariables; //re: the above comment about coding backwards to it, this line of code came first [after that comment]
    //need to populate uiVariables!
    //using rootLayout [and it's children ofc], I need to populate uiVariables!
    //maybe rootLayout _should_ be an array, however eg QGridLayout doesn't make sense as an array!!
    const QJsonArray &verticalUiVariables = rootLayout.value("verticalUiVariables").toArray();
    for(QJsonArray::const_iterator it = verticalUiVariables.constBegin(); it != verticalUiVariables.constEnd(); ++it)
    {
        const QJsonObject &currentUiVariableJsonObject = (*it).toObject();
        QString typeString = currentUiVariableJsonObject.value("typeString").toString();
        QString humanReadableNameForShowingFinalEndUser = currentUiVariableJsonObject.value("humanReadableNameForShowingFinalEndUser").toString();
        QString variableName = currentUiVariableJsonObject.value("variableName").toString();
        QString variableValue = currentUiVariableJsonObject.value("variableValue").toString();

        uiVariables.append(UIVariable(typeString, humanReadableNameForShowingFinalEndUser, variableName, variableValue));
    }

    //so now uiVariables is populated, time to iterate it and output shit (I guess this could be done during input _reading_, but eh stfu I can't think that pro. can probably refactor to that later ezily anyways -- hmm actually I think the above 'input reading' might be generic enough to even put in our interface/abstract-base-class for re-use in other sibling ui generator implementations (such as cli,qml,etc))

    QTemporaryDir outputDir;
    outputDir.setAutoRemove(false);
    QString outputPathWithSlashAppended = appendSlashIfNeeded(outputDir.path());

    QFile outputHeaderFile(outputPathWithSlashAppended + "SomeFuckingWidgetYo.h"); //TODOreq: parse the "ui name" from the inputFormat json shiz
    if(!outputHeaderFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "failed to open file:" << outputHeaderFile.fileName();
        return false;
    }
    QTextStream outputHeaderFileTextStream(&outputHeaderFile);

    QFile outputSourceFile(outputPathWithSlashAppended + "SomeFuckingWidgetYo.cpp"); //TODOreq: parse the "ui name" from the inputFormat json shiz
    if(!outputSourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "failed to open file:" << outputSourceFile.fileName();
        return false;
    }
    QTextStream outputSourceFileTextStream(&outputSourceFile);

    for(QList<UIVariable>::const_iterator it = uiVariables.constBegin(); it != uiVariables.constEnd(); ++it)
    {
        const UIVariable &currentUiVariable = (*it);
        streamOutUiVariableInstantiationCode(outputSourceFileTextStream, currentUiVariable);
    }
    qDebug() << "your output is here:" << outputDir.path();
    return true;
}
void QtWidgetsUiGenerator::streamOutUiVariableInstantiationCode(QTextStream &outputSourceFileTextStream, const UIVariable &uiVariable)
{
    if(uiVariable.Type == UIVariableType::LineEdit_String)
    {
        QString lineEditVariableName = uiVariable.VariableName + "LineEdit";
        outputSourceFileTextStream << TAB_QtWidgetsUiGenerator << "QLineEdit *" << lineEditVariableName << " = new QLineEdit();" << endl;
        outputSourceFileTextStream << TAB_QtWidgetsUiGenerator << "myLayout.addWidget(" << lineEditVariableName << ");" << endl;
        //TODOmb: connect to the line edit's signals? or do we wait until "ok" is pressed before we get the text results? either way works tbh. KISS [but do it right (aren't these contradictions?)]
    }
    //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh
}
