#include "comboboxwithautocompletionofexistingsignalsorslotsandautocompletionofargsifnewsignalorslot.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QMessageBox>
#include <QTextCursor>
#include <QLineEdit>

#define ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot_DUMMY_FILENAME "dummyfile.cpp"

//If the cursor is to the left of the open parenthesis, we use the default completer filled with full signal/slot signatures (they are also "combo box items"). When to the right of the open parenthesis (and to the left of the close parenthesis when it exists), we complete all types in project. When the close parenthesis is typed, we should try to parse the signal/slot (function) whenever the line edit changes. I ned "realtime" function validation because of the "signal/slot arg filler in" stuff in d=i that I forgot even existed but wowza obviously I want it. validation is prerequisite to being able to select variables for arg filling in. TODOreq: when no open or close parenthesis typed, function parse/validation should be on also. a simple no-arg function is assumed (this might already be in the function parsing logic that I'm about to refactor, I'm unsure)
//TODOreq: when a new arg type is used in a function declaration, it is impossible for ANY variable in the project to be able to satisfy it during the arg filling in stage (ignoring inheritence which is not implemented yet). What should I do then? The best I could come up with is some "auto" button that fills in all the arg fillers it can for you... and when you have a new/unknown type, that auto button creates members/properties of that new type and uses them. For existing/known types, it would use a member/property (OR PREFERABLY LOCAL, once that's implemented if ever xD wat) of the matching type if and only if there is exactly 1 member/property of that type... otherwise it leaves the arg filling in combo box for that arg in an initial/invalid/default/needs-user-decision state. Being able to declare arg types on the fly definitely makes sense in the class editor "signal/slot declaration"... but I'm starting to wonder if it makes sense during the signal-emission/slot-invocation "message editor" -- that "auto" button solution sounds kinda whack... but idk maybe it's a damn good solution. I'm trying to envision myself using it and idk it's hard to simulate the future like that :-P. After declaring a new signal/slot with a new arg type in the class editor, it might be a good idea to ask the user if they want corresponding members/properties created (to match this hypothetical "auto" button's functionality). Maybe s/auto/Guess ? Best-Guess? Bleh semanics... but I do need to decide before I'm able to continue this change that's become so much larger than antcipated xD
//^Would pressing that "auto" button to create members/properties of the new types create a "ghost" member/property until the ok button is pressed? because what if cancel is pressed and the new signal/slot is discarded? TODOreq
//^^Another thing to consider, which may or may not complement the "auto" button, is having non-modal "class editor" and "signals/slots message editor" dialogs... so if they type in a new arg type in the signal/slot message editor, they can switch to or open the class editor and add the member of that type themselves. I think however... err my instincts tell me that... changing from modal to non-modal would be a pretty large amount of work xD. A simple "auto" button would be easier and I could always do non-modal later...
//^^^For arg slot filling in, if filling in an arg for a type JUST declared in the signal/slot that was also just declared, the combo box of selectables should show a special item: "create member/property NewType *m_NewType0" (etc), which complements the "auto"/guess functionality (ie, does the same thing as it). In fact, the "auto"  button should just select that "special" combo box entry. Ideally the arg slot fillers themselves could be editable combo boxes TODOmb, but fuck me I can't think about that right now
//^^^^Although this is all good design etc, in the name of KISS I'm going to not implement any of it. This app has been in a non-compiling state for too long ("commit early, commit often" -- I think always in a compiling state is one of those but I break that often), so I'm just going to ignore "new arg types" altogether. The "parse/validate signal/slot syntax on keystroke (and update arg slot filling in combo boxes accordingly)" change is complex enough that it needs to be finished/committed before I focus on this "new arg TYPE" shit all these comments are about. So if there's a new arg TYPE in the function declaration, the "signal slot message dialog" will simply be un-accept()-able until a future commit. KISS/one-thing-at-a-time-fucker
//TODOreq: when cursor is immediately after the open parenthesis and nothing typed, ctrl+space should show all known types. I think libclang completion already does this. There's also the subsequent args to consider
//TODOreq: if a new signal declaration is used, and it has at least one new arg TYPE, pressing tab [to go to the slot line edit] should fill in a "handleSignal(...matching signature...)" slot for them (this is independent of the same idea as a user request/button). The reason being that they obviously must create a slot [with a matching/similar signature] since there are new arg types. Inheritence must be taken into consideration (a slot of a base type could be used? maybe but I think I need to cast before giving it to the signal idfk). The "handleSignal" text should be selected/highlighted because they most likely are going to change that part of the line edit
//TODOmb: ~200ms with no keystrokes before asking libclang to try to validate the signal/slot definition?
//TODOreq: A new arg type should be auto-completed when using it again on subsequent args of the same signal/slot signature. ex: someSlot(NewType arg0, NewT <- NewType should be completed. Yes we still have to ask them whether or not to make a new d=i class out of it (or if it's defined elsewhere), but that's irrelevant
//TODOreq: auto-completion of existing signals/slots should still work even when the user types "void " in front
//TODOreq: a more appropriate name for this class is now: ComboBox_with_AutoCompletionOfExistingFunctions_and_RealtimeSyntaxValidationOfNewFunctions_and_AutoCompletionOfNewFunctionArgs -- not to mention new function arg TYPES are accepted [and even auto-completed on subsequent uses wowow] xD
//^or maybe since that's a bit wordy, just LibClangFunctionParsingComboBox... and in the comments is where it's functionality is described using numbers/bullets
ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot(QWidget *parent)
    : QComboBox(parent)
    , LibClangFunctionDeclarationParser()
    , m_LastSeenIndex(0)
    , m_IsInitialized(true)
    //, m_ResultType(NoFunction)
    , m_SyntaxIsValid(false)
    , m_CompleterPopup(new QCompleter(this))
{
    insertItem(0, "");
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);
    setLineEdit(new LineEditWithPublicCursorRect(this));
    completer()->setCompletionMode(QCompleter::PopupCompletion);

    m_CompleterPopup->setCaseSensitivity(Qt::CaseInsensitive);
    m_CompleterPopup->setWidget(this);
    m_CompleterPopup->setCompletionMode(QCompleter::PopupCompletion);
    connect(m_CompleterPopup, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

    m_ClangIndex = clang_createIndex(1, 0);
    if(!m_ClangIndex)
    {
        m_IsInitialized = false;
        QMessageBox::critical(this, tr("Error"), tr("clang_createIndex failed"));
    }

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCurrentIndexChanged(int)));
    connect(this, SIGNAL(editTextChanged(QString)), this, SLOT(handleEditTextChanged(QString)));
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

    //bool useDefaultCompleterAndSyntaxCheckEachKeystrokeIfTrue_or_useAllExistingTypesInProjectCompleterAndDontSyntaxCheckEachKeystrokeIfFalse = false; //TODoreq: there exists a scenario where the close parenthesis is already typed but they're editting the args... in which case we want syntax checking and "existing types completion"... so yea this bool sucks I shouldn't combine the two

    bool useDefaultCompleter = false;

    int currentCursorPosition = lineEdit()->cursorPosition();
    QString lineEditText = lineEdit()->text();
    int indexOfOpenParenthesis = lineEditText.indexOf("(");
    if(indexOfOpenParenthesis < 0)
        useDefaultCompleter = true; //no open parenthesis yet? use default completer
    if(currentCursorPosition <= indexOfOpenParenthesis)
        useDefaultCompleter = true; //cursor is to the left of open parenthesis? use default completer
    int indexOfCloseParenthesis = lineEditText.indexOf(")");
    if(indexOfCloseParenthesis > -1 && currentCursorPosition > indexOfCloseParenthesis)
        useDefaultCompleter = true; //cursor is to the right of close parenthesis? use default completer

    if(!useDefaultCompleter)
    {
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
}
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::focusInEvent(QFocusEvent *e)
{
    m_CompleterPopup->setWidget(this);
    QComboBox::focusInEvent(e);
}
bool ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::syntaxIsValid() const
{
    return m_SyntaxIsValid;
}
QString ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::parsedFunctionName() const
{
    return m_SyntaxIsValid ? LibClangFunctionDeclarationParser::parsedFunctionName() : QString();
}
QList<QString> ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::newTypesSeenInParsedFunctionDeclaration() const
{
    return m_SyntaxIsValid ? LibClangFunctionDeclarationParser::newTypesSeenInFunctionDeclaration() : QList<QString>();
}
QList<FunctionArgumentTypedef> ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::parsedFunctionArguments() const
{
    return m_SyntaxIsValid ? LibClangFunctionDeclarationParser::parsedFunctionArguments() : QList<FunctionArgumentTypedef>();
}
QString ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::mostRecentSyntaxError() const
{
    return LibClangFunctionDeclarationParser::mostRecentError();
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

    int column = lineOfInterest.length()+1-token.length();

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
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::handleCurrentIndexChanged(int newIndex)
{
    if(newIndex == 0)
    {
        m_SyntaxIsValid = false; //just in case
        emit selectedFunctionChanged(NoFunction);
    }
    else
    {
        m_SyntaxIsValid = true; //just in case
        emit selectedFunctionChanged(ExistingFunction);
    }
}
void ComboBoxWithAutoCompletionOfExistingSignalsOrSlotsAndAutoCompletionOfArgsIfNewSignalOrSlot::handleEditTextChanged(QString newEditText)
{
    //selecting existing items brings us here, wtf? so the following is a hack: if the edit text changes but the current index does NOT, we know it's neither an existing item nor an empty/no result
    if(m_LastSeenIndex != currentIndex())
    {
        m_LastSeenIndex = currentIndex();
        return; //this was either an existing combo box item or the blank/empty one at index 0 at the top
    }

    if(newEditText.trimmed().isEmpty())
    {
        m_SyntaxIsValid = false; //just in case
        emit selectedFunctionChanged(NoFunction);
        return;
    }

    bool checkSyntax = newEditText.contains("(") && newEditText.contains(")");
    if(!checkSyntax)
        checkSyntax = (!newEditText.contains("(")) && (!newEditText.contains(")"));
    if(checkSyntax)
    {
        //if we get here, there are either no parenthesis at all, or there is a close parenthesis to match the open parenthesis
        parseFunctionDeclaration(newEditText, m_AllKnownTypes);
        m_SyntaxIsValid = !hasError();
    }
    else
        m_SyntaxIsValid = false; //they're typing the function arguments and have yet to hit the ")"

    emit selectedFunctionChanged(TypedInFunction);
}
