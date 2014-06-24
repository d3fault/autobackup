#include "subjectmattermasteryhelpercli.h"

#include <QCoreApplication>
#include <QStringList>

#include "../../subjectmattermasteryhelper.h"
#include "standardinputnotifier.h"

SubjectMatterMasteryHelperCli::SubjectMatterMasteryHelperCli(QObject *parent)
    : QObject(parent)
    , m_SubjectMatterMasteryHelper(new SubjectMatterMasteryHelper(this))
    , m_StandardInputNotifier(new StandardInputNotifier(this))
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    QStringList argz = qApp->arguments();
    if(argz.length() != 2)
    {
        usage();
        handleQuitRequested();
        return;
    }

    connect(m_StandardInputNotifier, SIGNAL(standardInputReceivedLine(QString)), this, SLOT(handleStandardInputReceivedLine(QString)));

    connect(m_SubjectMatterMasteryHelper, SIGNAL(questionAsked(QString)), this, SLOT(handleQuestionAsked(QString)));
    connect(this, SIGNAL(answeredQuestion(QString)), m_SubjectMatterMasteryHelper, SLOT(questionAnswered(QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(questionAnsweredGraded(bool, QString)), this, SLOT(handleQuestionAnsweredGraded(bool, QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(testingFinished()), this, SLOT(handleQuitRequested()));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(m_SubjectMatterMasteryHelper, SIGNAL(quitRequested()), this, SLOT(handleQuitRequested()));

    QMetaObject::invokeMethod(m_SubjectMatterMasteryHelper, "startSubjectMatterMasteryHelper", Q_ARG(QString, argz.at(1))); //OT'ish: using invokeMethod for one offs is more memory conservative, because the connection is not persistent. i wonder if it's slower though TODOoptimization?
}
void SubjectMatterMasteryHelperCli::usage()
{
    QString usageStr =  "Usage: SubjectMatterMasteryHelperCli filenameOfSubjectMatter\n\n"
                        "\tfilenameOfSubjectMatter should be a file with a question on a line, an answer on a line, repeating for as many questions/answers as you want (every question must have an answer, otherwise it is an error)";
    m_StdOut << usageStr << endl;
}
void SubjectMatterMasteryHelperCli::handleQuestionAsked(const QString &question)
{
    m_StdOut << ("Question: " + question) << endl;
}
void SubjectMatterMasteryHelperCli::handleStandardInputReceivedLine(const QString &standardInputLine)
{
    if(standardInputLine.isEmpty())
        return; //ignore accidental enter presses
    emit answeredQuestion(standardInputLine);
}
void SubjectMatterMasteryHelperCli::handleQuestionAnsweredGraded(bool answerAttemptWasCorrect, const QString &answerIfAttemptWasIncorrect)
{
    if(answerAttemptWasCorrect)
    {
        m_StdOut << "Correct" << endl;
    }
    else
    {
        m_StdOut << ("Incorrect. The answer is: " + answerIfAttemptWasIncorrect) << endl;
    }
}
void SubjectMatterMasteryHelperCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
void SubjectMatterMasteryHelperCli::handleQuitRequested()
{
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
