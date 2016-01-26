#include "comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QMessageBox>
#include <QTextCursor>
#include <QLineEdit>
#include <QDebug>

#include "lineeditwithpubliccursorrect.h"

#define ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_DUMMY_FILENAME "dummyfile.cpp"

//So yea if the cursor is to the left of the open parenthesis, we use the default completer filled with full signal/slot signatures (they are also "combo box items"). When to the right of the open parenthesis, we complete all types in project. When to the right of the close parenthesis, we shouldn't complete anything at all, BUT just to KISS we should just go back to using the default completer TODoreq
//TODOreq: when cursor is immediately after the open parenthesis and nothing typed, ctrl+space should show all known types. I think libclang completion already does this. There's also the subsequent args to consider
//TODOreq: A new arg type should be auto-completed when using it again on subsequent args of the same signal/slot signature. ex: someSlot(NewType arg0, NewT <- NewType should be completed. Yes we still have to ask them whether or not to make a new d=i class out of it (or if it's defined elsewhere), but that's irrelevant
ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent)
    : QComboBox(parent)
    , m_CompleterPopup(new QCompleter(this))
{
    QStringList existingSignalsOrSlots;
    existingSignalsOrSlots << "someSlot0()" << "someSlot1(int someArg)";
    insertItem(0, "");
    insertItems(count(), existingSignalsOrSlots);

    m_AllKnownTypes << "CustomType0" << "BlahType";

    setEditable(true);
    setLineEdit(new LineEditWithPublicCursorRect(this));
    completer()->setCompletionMode(QCompleter::PopupCompletion);

    m_CompleterPopup->setCaseSensitivity(Qt::CaseInsensitive);
    m_CompleterPopup->setWidget(this);
    m_CompleterPopup->setCompletionMode(QCompleter::PopupCompletion);
    connect(m_CompleterPopup, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

    m_ClangIndex = clang_createIndex(1, 0);
    if(!m_ClangIndex)
    {
        QMessageBox::critical(this, tr("Error"), tr("clang_createIndex failed"));
        close();
    }
    QSize currentSize = size();
    currentSize.setWidth(400);
    resize(currentSize);
}
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::keyPressEvent(QKeyEvent *e)
{
    if(m_CompleterPopup->popup()->isVisible())
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if(!isShortcut)
        QComboBox::keyPressEvent(e);

    if(lineEdit()->hasSelectedText())
        return; //don't auto-complete when there's selected text. if we tried to, it breaks text selecting completely (at least via arrow keys. didn't try with mouse) since we hackily use "text selecting" (and more importantly, deselecting) to get the completion prefix

    int currentCursorPosition = lineEdit()->cursorPosition();
    QString lineEditText = lineEdit()->text();
    int indexOfOpenParenthesis = lineEditText.indexOf("(");
    if(indexOfOpenParenthesis < 0)
        return; //no open parenthesis yet? use default completer
    if(currentCursorPosition <= indexOfOpenParenthesis)
        return; //cursor is to the left of open parenthesis? use default completer
    int indexOfCloseParenthesis = lineEditText.indexOf(")");
    if(indexOfCloseParenthesis > -1 && currentCursorPosition > indexOfCloseParenthesis)
        return; //curso is to the right of close parenthesis? use default completer

    //if we get here, we're inside a signal/slot arg signature and want to auto-complete all types in project

    static QString endOfWordCharacters("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
    bool hasModifier = (e->modifiers() != Qt::NoModifier);

    //QTextCursor tc =  textCursor();
    //tc.select(QTextCursor::WordUnderCursor);
    //QString completionPrefix = tc.selectedText();

    //ugly/hack, why the fuck doesn't QLineEdit have a QTextCursor like QTextEdit :-(
    lineEdit()->cursorWordBackward(true);
    QString completionPrefix = lineEdit()->selectedText();
    lineEdit()->deselect(); //I hope this doesn't cause flashing. it didn't in the QTextEdit version. Bah after testing I think it might. I see the cursor jumping left and right but then again that's pretty normal on this 1fps magnetic ink screen xD
    lineEdit()->setCursorPosition(currentCursorPosition);

    if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || endOfWordCharacters.contains(e->text().right(1))))
    {
        m_CompleterPopup->popup()->hide();
        return;
    }

    populateCompleterPopupViaClangCodeComplete(lineEditText, completionPrefix);

    if(completionPrefix != m_CompleterPopup->completionPrefix())
    {
        m_CompleterPopup->setCompletionPrefix(completionPrefix);
        m_CompleterPopup->popup()->setCurrentIndex(m_CompleterPopup->completionModel()->index(0, 0));
    }
    QRect cr = static_cast<LineEditWithPublicCursorRect*>(lineEdit())->publicCursorRect();
    cr.setWidth(m_CompleterPopup->popup()->sizeHintForColumn(0) + m_CompleterPopup->popup()->verticalScrollBar()->sizeHint().width());
    m_CompleterPopup->complete(cr);
}
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::focusInEvent(QFocusEvent *e)
{
    m_CompleterPopup->setWidget(this);
    QComboBox::focusInEvent(e);
}
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::populateCompleterPopupViaClangCodeComplete(const QString &lineEditText, const QString &token)
{
    QString sourceCode;
    int line = 1;
    Q_FOREACH(const QString &knownType, m_AllKnownTypes)
    {
        sourceCode.append("typedef int " + knownType + ";\n");
    }
    line += m_AllKnownTypes.size();
    QString lineOfInterest;
    lineOfInterest.append("static ");
    if(!lineEditText.trimmed().startsWith("void")) //typing void in the line edit is optional
        lineOfInterest.append("void ");
    lineOfInterest.append(lineEditText);
    sourceCode.append(lineOfInterest);

    CXUnsavedFile unsavedFiles[1];
    unsavedFiles[0].Filename = ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_DUMMY_FILENAME;
    std::string docStdString = sourceCode.toStdString();
    unsavedFiles[0].Contents = docStdString.c_str();
    unsavedFiles[0].Length = docStdString.length();

    CXTranslationUnit translationUnit = clang_parseTranslationUnit(m_ClangIndex, ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_DUMMY_FILENAME, 0, 0, unsavedFiles, 1, CXTranslationUnit_PrecompiledPreamble);
    if(!translationUnit)
    {
        QMessageBox::critical(this, tr("Error"), tr("parseTranslationUnit failed"));
        m_CompleterPopup->setModel(new QStringListModel(m_CompleterPopup));
        return;
    }

    //clang_reparseTranslationUnit(u, 0, 0, 0);

    qDebug() << "Source:";
    qDebug() << sourceCode;
    qDebug() << "Line:" << line;
    int column = lineOfInterest.length()+1-token.length();
    qDebug() << "Column:" << column;

    CXCodeCompleteResults* codeCompleteResults = clang_codeCompleteAt(translationUnit, ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_DUMMY_FILENAME, line, column, unsavedFiles, 1, 0);
    if(!codeCompleteResults)
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not complete"));
        m_CompleterPopup->setModel(new QStringListModel(m_CompleterPopup));
        return;
    }

    QStringList codeCompletionEntries;
    for(unsigned i = 0; i < codeCompleteResults->NumResults; ++i)
    {
        const CXCompletionString& str = codeCompleteResults->Results[i].CompletionString;
        for(unsigned j = 0; j < clang_getNumCompletionChunks(str); ++j)
        {
            if(clang_getCompletionChunkKind(str, j) != CXCompletionChunk_TypedText)
                continue;

            const CXString& out = clang_getCompletionChunkText(str, j);
            const char* codeCompletionEntry = clang_getCString(out);
            QString codeCompletionEntryQString = QString::fromLatin1(codeCompletionEntry, strlen(codeCompletionEntry));
            if(!codeCompletionEntryQString.trimmed().isEmpty())
                codeCompletionEntries.append(codeCompletionEntry);
        }
    }

    m_CompleterPopup->setModel(new QStringListModel(codeCompletionEntries, m_CompleterPopup));
    clang_disposeCodeCompleteResults(codeCompleteResults);
}
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::insertCompletion(const QString &completion)
{
    if(m_CompleterPopup->widget() != this)
        return;

//    QTextCursor tc = textCursor();
//    tc.select(QTextCursor::WordUnderCursor);
//    tc.removeSelectedText(); //capitalization may have been wrong, since we're doing case insensitive completing
//    tc.insertText(completion);
//    setTextCursor(tc);

    lineEdit()->cursorWordBackward(true);
    //insert implies delete. lineEdit()->del(); //capitalization may have been wrong, since we're doing case insensitive completing
    lineEdit()->insert(completion);
}
