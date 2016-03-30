#ifndef COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H
#define COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H

#include <QComboBox>
#include "libclangfunctiondeclarationparser.h"

#include <clang-c/Index.h>

class QCompleter;

class ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot
        : public QComboBox
        , protected LibClangFunctionDeclarationParser //protected because we don't want to expose hasError. syntaxIsValid wraps hasError (with additional combobox specific functionality)
{
    Q_OBJECT
    Q_ENUMS(ResultType)
public:
    enum ResultType
    {
          NoFunction = 0
        , TypedInFunction = 1
        , ExistingFunction = 2
    };

    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent = 0);
    bool isInitialized() const { return m_IsInitialized; }
    //ResultType resultType() const { return m_ResultType; }
    void insertKnownTypesExcludingBuiltIns(const QStringList &knownTypesExcludingBuiltIns) { m_AllKnownTypesExcludingBuiltIns.append(knownTypesExcludingBuiltIns); }
    bool syntaxIsValid() const;

    //only valid when resultType == NewResult && m_SyntaxIsValid == true
    QString parsedFunctionName() const;
    QList<ParsedTypeInstance> parsedFunctionArguments() const;
    QList<QString> newTypesSeenInParsedFunctionDeclaration() const;
    QString mostRecentSyntaxError() const;
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    int m_LastSeenIndex;
    bool m_IsInitialized;
    //ResultType m_ResultType;
    bool m_SyntaxIsValid;
    QStringList m_AllKnownTypesExcludingBuiltIns;
    QCompleter *m_CompleterPopup;

    CXIndex m_ClangIndex;

    void parseFunctionDeclarationAndSetSyntaxIsValidAccordingly(const QString &lineEditText);
    void populateCompleterPopupViaClangCodeComplete(const QString &lineEditText, const QString &token);
signals:
    void selectedFunctionChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType resultType/*, bool syntaxIsValid*/); //is emitted when resultType changes, a different existing function is selected (index changes), syntax is valid changes, or even when neither of those change but the parsed function name/arguments does change ("s" becaomes "so" becomes "som" ultimately becomes "someSlot")
private slots:
    void insertCompletion(const QString &completion);
    void handleCurrentIndexChanged(int newIndex);
    void handleEditTextChanged(QString newEditText);
};


#include <QLineEdit>

class LineEditWithPublicCursorRect : public QLineEdit
{
public:
    explicit LineEditWithPublicCursorRect(QWidget* parent=0)
        : QLineEdit(parent)
    { }

    QRect publicCursorRect() const
    {
        return cursorRect();
    }
};

#endif // COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H
