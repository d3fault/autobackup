#include "subjectmattermasteryhelpercli.h"

#include <QCoreApplication>
#include <QStringList>

#include "../../subjectmattermasteryhelper.h"
#include "standardinputnotifier.h"

#define SubjectMatterMasteryHelperCli_ANSWERS_TOO_LONG_USE_MULTIPLE_CHOICE_CLI_ARG "--answers-too-long-use-multiple-choice"
#define SubjectMatterMasteryHelperCli_DONT_RANDOMIZE_CLI_ARG "--dont-randomize"

SubjectMatterMasteryHelperCli::SubjectMatterMasteryHelperCli(QObject *parent)
    : QObject(parent)
    , m_SubjectMatterMasteryHelper(new SubjectMatterMasteryHelper(this))
    , m_StandardInputNotifier(new StandardInputNotifier(this))
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    QStringList argz = qApp->arguments();
    argz.takeFirst(); //filepath

    bool answersTooLongUseMultipleChoice = false;
    int answerTooLongUseMultipleChoiceArgIndex = argz.indexOf(SubjectMatterMasteryHelperCli_ANSWERS_TOO_LONG_USE_MULTIPLE_CHOICE_CLI_ARG);
    if(answerTooLongUseMultipleChoiceArgIndex > -1)
    {
        answersTooLongUseMultipleChoice = true;
        argz.removeAt(answerTooLongUseMultipleChoiceArgIndex);
    }

    bool dontRandomize = false;
    int dontRandomizeArgIndex = argz.indexOf(SubjectMatterMasteryHelperCli_DONT_RANDOMIZE_CLI_ARG);
    if(dontRandomizeArgIndex > -1)
    {
        dontRandomize = true;
        argz.removeAt(dontRandomizeArgIndex);
    }

    if(argz.isEmpty()) //there should be exactly 1 arg left: the filenameOfSubjectMatter
    {
        usage();
        handleQuitRequested();
        return;
    }

    m_StandardInputNotifier->setNotifyOnEmptyInput(true);
    connect(m_StandardInputNotifier, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceivedLine(QString)));

    connect(m_SubjectMatterMasteryHelper, SIGNAL(questionAsked(QString)), this, SLOT(handleQuestionAsked(QString)));
    connect(this, SIGNAL(answeredQuestion(QString)), m_SubjectMatterMasteryHelper, SLOT(questionAnswered(QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(questionAnsweredGraded(bool, QString)), this, SLOT(handleQuestionAnsweredGraded(bool, QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(testingFinished()), this, SLOT(handleQuitRequested()));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(quitRequested()), this, SLOT(handleQuitRequested()));

    QMetaObject::invokeMethod(m_SubjectMatterMasteryHelper, "startSubjectMatterMasteryHelper", Q_ARG(QString, argz.first()), Q_ARG(bool, answersTooLongUseMultipleChoice), Q_ARG(bool, dontRandomize)); //OT'ish: using invokeMethod for one offs is more memory conservative, because the connection is not persistent. i wonder if it's slower though TODOoptimization?
}
void SubjectMatterMasteryHelperCli::usage()
{
    QString usageStr =  "Usage: SubjectMatterMasteryHelperCli filenameOfSubjectMatter [options]\n\n"
                        "\tfilenameOfSubjectMatter should be a file with a question on a line, an answer on a line, repeating for as many questions/answers as you want (every question must have an answer, otherwise it is an error)\n\nOptions:\n\n" SubjectMatterMasteryHelperCli_ANSWERS_TOO_LONG_USE_MULTIPLE_CHOICE_CLI_ARG "\n\tIf the answers are so long that they can't accurately be typed (with proper punctuation etc), this mode does multiple choice using randomly selected other answers as the other/wrong choices\n\n" SubjectMatterMasteryHelperCli_DONT_RANDOMIZE_CLI_ARG "\n\tPresent the questions in the same order that they appear in filenameOfSubjectMatter (the default behavior is to randomize their order)";
    m_StdOut << usageStr << endl;
}
void SubjectMatterMasteryHelperCli::handleQuestionAsked(const QString &question)
{
    m_StdOut << ("Question: " + question) << endl;
}
void SubjectMatterMasteryHelperCli::handleStandardInputReceivedLine(const QString &standardInputLine)
{
    emit answeredQuestion(standardInputLine);
}
void SubjectMatterMasteryHelperCli::handleQuestionAnsweredGraded(bool answerAttemptWasCorrect, const QString &answerIfAttemptWasIncorrect)
{
    if(answerAttemptWasCorrect)
    {
        m_StdOut << "Correct!" << endl << endl;
    }
    else
    {
        m_StdOut << ("Incorrect. The answer is: " + answerIfAttemptWasIncorrect) << endl << endl;
    }
}
void SubjectMatterMasteryHelperCli::handleO(const QString &msg)
{
    m_StdOut << msg;
    m_StdOut.flush();
}
void SubjectMatterMasteryHelperCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void SubjectMatterMasteryHelperCli::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
