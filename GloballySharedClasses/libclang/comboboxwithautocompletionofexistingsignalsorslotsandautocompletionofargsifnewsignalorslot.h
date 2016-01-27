#ifndef COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H
#define COMBOBOXWITHAUTOCOMPLETIONOFEXISTINGSIGNALSORSLOTSANDAUTOCOMPLETIONOFARGSIFNEWSIGNALORSLOT_H

#include <QComboBox>

#include <clang-c/Index.h>

class QCompleter;

class ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot : public QComboBox
{
    Q_OBJECT
public:
    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent = 0);
    bool isValid() const { return m_IsValid; }
    void insertKnownTypes(const QStringList &knownTypes) { m_AllKnownTypes.append(knownTypes); }
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    bool m_IsValid;
    QStringList m_AllKnownTypes;
    QCompleter *m_CompleterPopup;

    CXIndex m_ClangIndex;

    void populateCompleterPopupViaClangCodeComplete(const QString &lineEditText, const QString &token);
private slots:
    void insertCompletion(const QString &completion);
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
