#ifndef COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H
#define COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H

#include <QComboBox>

#include <clang-c/Index.h>

class QCompleter;

class ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot : public QComboBox
{
    Q_OBJECT
    Q_ENUMS(ResultState)
public:
    enum ResultState
    {
          NoResult = 0
        , NewResult = 1
        , ExistingResult = 2
    };

    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent = 0);
    bool isInitialized() const { return m_IsInitialized; }
    ResultState resultState() const { return m_ResultState; }
    void insertKnownTypes(const QStringList &knownTypes) { m_AllKnownTypes.append(knownTypes); }
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    bool m_IsInitialized;
    ResultState m_ResultState;
    QStringList m_AllKnownTypes;
    QCompleter *m_CompleterPopup;

    CXIndex m_ClangIndex;

    void populateCompleterPopupViaClangCodeComplete(const QString &lineEditText, const QString &token);
    void setResultState(ResultState newResultState);
signals:
    void resultStateChanged(ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ResultState newResultState);
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
