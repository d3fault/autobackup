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
          NoResult = 0
        , NewResult = 1
        , ExistingResult = 2
    };

    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent = 0);
    bool isInitialized() const { return m_IsInitialized; }
    ResultType resultType() const { return m_ResultType; }
    void insertKnownTypes(const QStringList &knownTypes) { m_AllKnownTypes.append(knownTypes); }
    bool syntaxIsValid() const;

    //only valid when m_SyntaxIsValid == true
    QString functionName() const;
    QList<QString> newTypesSeenInFunctionDeclaration() const;
    QList<FunctionArgumentTypedef> functionArguments() const;
    QString mostRecentSyntaxError() const;
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    bool m_IsInitialized;
    ResultType m_ResultType;
    bool m_SyntaxIsValid;
    QStringList m_AllKnownTypes;
    QCompleter *m_CompleterPopup;
    QString m_ParsedFunctionNameForDetectingChanges;
    QList<FunctionArgumentTypedef> m_ParsedFunctionArgumentsForDetectingChanges;

    CXIndex m_ClangIndex;

    void checkSyntaxAndSetSyntaxIsValidAccordingly(const QString &lineEditText);
    void populateCompleterPopupViaClangCodeComplete(const QString &lineEditText, const QString &token);
    void setResultType(ResultType newResultType);
    void setSyntaxIsValid(bool syntaxIsValid);
    void setParsedFunctionName(const QString &newParsedFunctionName);
    void setParsedFunctionArguments(const QList<FunctionArgumentTypedef> &newParsedFunctionArguments);
signals:
    void resultTypeChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultType newResultType);
    void syntaxIsValidChanged(bool syntaxIsValid);
    void parsedFunctionNameChanged(QString newParsedFunctionName);
    void parsedFunctionArgumentsChanged(QList<FunctionArgumentTypedef> newParsedFunctionArguments);
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
