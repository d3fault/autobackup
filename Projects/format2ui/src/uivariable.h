#ifndef UIVARIABLE_H
#define UIVARIABLE_H

class UIVariableType
{
public:
    //these all describe INPUTS on the final gui, we need a special type to represent OUTPUTS on the final gui (they will be "static", such as QLabel, not doing any special processing [yet? I suppose dynamic shiz is psbl mindfuck.jpg], but possibly giving the user some very useful instructions, for example)
    enum UIVariableTypeEnum
    {
          LineEdit_String = 0
        , ScrollBox_Int = 1
        , PlainTextEdit_StringList = 2
        , PlainTextEdit_IntList = 3
        //etc! can get very complex! custom widgets ofc
    };
};
struct UIVariable
{
    UIVariable(const QString &typeString, const QString &humanReadableNameForShowingFinalEndUser, const QString &variableName, const QString &variableValue)
        : Type(uIVariableTypeStringToType(typeString))
        , HumanReadableNameForShowingFinalEndUser(humanReadableNameForShowingFinalEndUser)
        , VariableName(variableName)
        , VariableValue(variableValue)
    { }

    UIVariableType::UIVariableTypeEnum Type;
    QString HumanReadableNameForShowingFinalEndUser;
    QString VariableName;
    QString VariableValue;
private:
    static UIVariableType::UIVariableTypeEnum uIVariableTypeStringToType(const QString &typeString)
    {
        if(typeString == "String")
            return UIVariableType::LineEdit_String;
        //if(typeString == "StringArray")
            //return UIVariableType::ExpandingListOfLineEdits_StringArray;
        if(typeString == "StringList")
            return UIVariableType::PlainTextEdit_StringList; //one plain text edit is easier to deal with than many line edits, but I do think many line edits is the better way to do it
        //etc!
        qFatal("invalid type string");
        return UIVariableType::LineEdit_String;
    }
};

#endif // UIVARIABLE_H
