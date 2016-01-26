#include "comboboxwithtwodifferentcompletertypesdependingonwheretextcursoris.h"

#include <QCompleter>
#include <QLineEdit>

//In d=i, I am in a situation where I want a combo box to auto complete existing signals/slots (for the "current" object), but I also want that same combo box to auto complete arg types if they don't choose an existing signal/slot and are defining a new signal/slot on the fly. One completer has signals or slots, the other completer has "all types in project". This prototype is to see if hot swapping the completer out even works. Which completer is being used should depend on where the text cursor is relative to an open parenthesis "(". If there is no parenthesis, the text cursor is considered "on the left" and so we auto complete with signals/slots
ComboBoxWithTwoDifferentCompleterTypesDependingOnWhereTextCursorIs::ComboBoxWithTwoDifferentCompleterTypesDependingOnWhereTextCursorIs(QWidget *parent)
    : QComboBox(parent)
{
    a; //this app segfaults, so i'm intentionally breaking compilation. I didn't also think to parse out the signal/slot name in front of the completed arg type completions. new prototype inc
    m_SignalsOrSlotsCompletionEntries << "someSlot1()" << "someSlot2(int x)";
    m_AllTypesInProjectCompletionEntries << "CustomClass1" << "CustomClass2";

    insertItem(0, ""); //blank item for them to type new signal/slot in
    insertItems(count(), m_SignalsOrSlotsCompletionEntries);

    setEditable(true);
    setInsertPolicy(NoInsert); //clang must parse/verify the new signal/slot signature first. they'll show up only next time the signal/slot message dialog is shown

    m_LineEdit = lineEdit();

    m_Default_aka_SignalsOrSlots_Completer = completer(); //thx :)
    m_Default_aka_SignalsOrSlots_Completer->setCompletionMode(QCompleter::PopupCompletion); //or maybe unfiltered? or maybe it's a d=i setting/pref? I wish I chould have inline AND popup :(... ctrl+space to use or w/e

    m_AllTypesInProject_Completer = new QCompleter(m_AllTypesInProjectCompletionEntries, this);
    m_AllTypesInProject_Completer->setCompletionMode(QCompleter::PopupCompletion); //pretty tempted to leave it inline... but again: wish I could have inline _AND_ popup :-P

    connect(m_LineEdit, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(handleCursorPositionChanged(int,int)));
}
void ComboBoxWithTwoDifferentCompleterTypesDependingOnWhereTextCursorIs::handleCursorPositionChanged(int oldPos, int newPos)
{
    Q_UNUSED(oldPos)
    QCompleter *currentCompleter = completer();
    int indexOfOpenParenthesis = m_LineEdit->text().indexOf("(");
    if(indexOfOpenParenthesis < 0 || newPos <= indexOfOpenParenthesis)
    {
         //no open parenthesis,or to the left of open parenthesis: ensure signals/slots completer is being used and set it if it isn't
        if(currentCompleter != m_Default_aka_SignalsOrSlots_Completer)
            setCompleter(m_Default_aka_SignalsOrSlots_Completer);
        return;
    }
    //deductive reasoning, we must be to the right of the open parenthesis if we get here: ensure allTypesInProject completer is being used and set it if it isn't
    if(currentCompleter != m_AllTypesInProject_Completer)
        setCompleter(m_AllTypesInProject_Completer);
}
