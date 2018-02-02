#include "iuigenerator.h"

#include <QJsonArray>

void IUIGenerator::populateUiVariables(const QJsonObject &inputJsonObject, QList<UIVariable> *out_uiVariables)
{
    //a ui format input is a series of layouts (containing "widgets" therein)
    //so as not to digress too fucking much, I'll start with a QVBoxLayout as my "root" layout. I use QVBoxLayout the most
    const QJsonObject &rootLayout = inputJsonObject.value("rootLayout").toObject();
    //I could have returned an array, but dealing with objects is ezier imo -- (ed note: we return an array but later actually)
    //maybe rootLayout _should_ be an array, however eg QGridLayout doesn't make sense as an array!!
    const QJsonArray &verticalUiVariables = rootLayout.value("verticalUiVariables").toArray();
    for(QJsonArray::const_iterator it = verticalUiVariables.constBegin(); it != verticalUiVariables.constEnd(); ++it)
    {
        const QJsonObject &currentUiVariableJsonObject = (*it).toObject();
        QString typeString = currentUiVariableJsonObject.value("typeString").toString();
        QString humanReadableNameForShowingFinalEndUser = currentUiVariableJsonObject.value("humanReadableNameForShowingFinalEndUser").toString();
        QString variableName = currentUiVariableJsonObject.value("variableName").toString();
        QString variableValue = currentUiVariableJsonObject.value("variableValue").toString();

        out_uiVariables->append(UIVariable(typeString, humanReadableNameForShowingFinalEndUser, variableName, variableValue));
    }
}
