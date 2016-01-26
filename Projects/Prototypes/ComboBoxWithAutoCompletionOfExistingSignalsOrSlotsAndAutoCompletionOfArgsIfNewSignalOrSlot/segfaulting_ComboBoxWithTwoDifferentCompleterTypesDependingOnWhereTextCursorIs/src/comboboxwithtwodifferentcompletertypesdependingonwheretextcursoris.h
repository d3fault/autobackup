#ifndef COMBOBOXWITHTWODIFFERENTCOMPLETERTYPESDEPENDINGONWHERETEXTCURSORIS_H
#define COMBOBOXWITHTWODIFFERENTCOMPLETERTYPESDEPENDINGONWHERETEXTCURSORIS_H

#include <QComboBox>

#include <QStringList>

class ComboBoxWithTwoDifferentCompleterTypesDependingOnWhereTextCursorIs : public QComboBox
{
    Q_OBJECT
public:
    ComboBoxWithTwoDifferentCompleterTypesDependingOnWhereTextCursorIs(QWidget *parent = 0);
private:
    QStringList m_SignalsOrSlotsCompletionEntries;
    QStringList m_AllTypesInProjectCompletionEntries;

    QLineEdit *m_LineEdit;

    QCompleter *m_Default_aka_SignalsOrSlots_Completer;
    QCompleter *m_AllTypesInProject_Completer;
private slots:
    void handleCursorPositionChanged(int oldPos,int newPos);
};

#endif // COMBOBOXWITHTWODIFFERENTCOMPLETERTYPESDEPENDINGONWHERETEXTCURSORIS_H
