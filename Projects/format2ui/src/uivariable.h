#ifndef UICOLLECTOR_H
#define UICOLLECTOR_H

#include <QJsonObject>

class UICollectorType
{
public:
    //these all describe INPUTS on the final gui, we need a special type to represent OUTPUTS on the final gui (they will be "static", such as QLabel, not doing any special processing [yet? I suppose dynamic shiz is psbl mindfuck.jpg], but possibly giving the user some very useful instructions, for example)
    enum UIVariableTypeEnum
    {
          Widget = 0
        , LineEdit_String = 1
        , ScrollBox_Int = 2
        , PlainTextEdit_StringList = 3
        , PlainTextEdit_IntList = 4
        , WidgetList = 5
        //etc! can get very complex! custom widgets ofc
    };
};
//TODOmb: s/UiCollector/UiVarCollector ?
//helper methods around a QJsonObject
class UICollector : public QJsonObject
{
public:
    UICollector()=delete;
    UICollector(const QJsonObject &other)
        : QJsonObject(other)
    {
        initUiCollector();
    }
#if 0
    UIVariable(const QString &typeString, const QString &humanReadableNameForShowingFinalEndUser, const QString &variableName, const QString &variableValue)
        : Type(uIVariableTypeStringToType(typeString))
        , HumanReadableNameForShowingFinalEndUser(humanReadableNameForShowingFinalEndUser)
        , VariableName(variableName)
        , VariableValue(variableValue)
    { }
#endif

    //Widget
    QString name() const { return value("name").toString(); }
    QJsonObject rootLayout() const { return value("rootLayout").toObject(); }

    //Non-Widget
    QString humanReadableNameForShowingFinalEndUser() const { return value("humanReadableNameForShowingFinalEndUser").toString(); }
    QString variableName() const { return value("variableName").toString(); }
    QString variableValue() const { return value("variableValue").toString(); }


    UICollectorType::UIVariableTypeEnum Type;
    //QString HumanReadableNameForShowingFinalEndUser;
    //QString VariableName;
    //QString VariableValue;
private:
    void initUiCollector()
    {
        QString typeString = value("typeString").toString();
        if(typeString.isEmpty())
            typeString = "Widget";
        Type = uIVariableTypeStringToType(typeString);
    }
    static UICollectorType::UIVariableTypeEnum uIVariableTypeStringToType(const QString &typeString)
    {
        if(typeString == "Widget")
            return UICollectorType::Widget;
        if(typeString == "String")
            return UICollectorType::LineEdit_String;
        //if(typeString == "StringArray")
            //return UIVariableType::ExpandingListOfLineEdits_StringArray; //TODOreq: yes call it this (not the StringArray part), but not until after the refactor
        if(typeString == "StringList")
            return UICollectorType::PlainTextEdit_StringList; //one plain text edit is easier to deal with than many line edits, but I do think many line edits is the better way to do it
        //etc!
        qFatal("invalid type string");
        return UICollectorType::LineEdit_String;
    }
};

#endif // UICOLLECTOR_H
