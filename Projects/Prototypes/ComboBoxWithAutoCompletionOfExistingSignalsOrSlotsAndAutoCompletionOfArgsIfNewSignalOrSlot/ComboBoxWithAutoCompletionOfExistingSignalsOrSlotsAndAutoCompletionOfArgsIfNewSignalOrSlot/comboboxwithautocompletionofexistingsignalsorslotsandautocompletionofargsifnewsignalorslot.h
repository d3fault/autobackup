#ifndef ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_H
#define ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_H

#include <QComboBox>

#include <clang-c/Index.h>

class QCompleter;

class ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot : public QComboBox
{
    Q_OBJECT
public:
    ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent = 0);
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
private:
    QStringList m_AllKnownTypes;
    QCompleter *m_CompleterPopup;

    CXIndex m_ClangIndex;

    void populateCompleterPopupViaClangCodeComplete(const QString &lineEditText, const QString &token);
private slots:
    void insertCompletion(const QString &completion);
};

#endif // ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_H
