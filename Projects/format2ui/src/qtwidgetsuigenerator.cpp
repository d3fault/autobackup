#include "qtwidgetsuigenerator.h"

#include <QTemporaryDir>
#include <QFile>
#include <QDebug>

bool QtWidgetsUiGenerator::generateUi(const QJsonObject &jsonUiFormatInput)
{
    QList<UIVariable> uiVariables;
    populateUiVariables(jsonUiFormatInput, &uiVariables);

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
        outputSourceFileTextStream << TAB_format2ui << "QLineEdit *" << lineEditVariableName << " = new QLineEdit();" << endl;
        outputSourceFileTextStream << TAB_format2ui << lineEditVariableName << "->setPlaceholderText(\"" << uiVariable.HumanReadableNameForShowingFinalEndUser << "\");" << endl; //a QLabel off to the left would be better (use buddying, too!)
        outputSourceFileTextStream << TAB_format2ui << "myLayout.addWidget(" << lineEditVariableName << ");" << endl;
        //TODOmb: connect to the line edit's signals? or do we wait until "ok" is pressed before we get the text results? either way works tbh. KISS [but do it right (aren't these contradictions?)]
    }
    //TODOreq: handle other types. could organize this much better ofc. would be great if we could use use pure virtuals to break compilation whenever any 1 ui generator doesn't implement any 1 UiVariableType. sounds ez tbh
}
