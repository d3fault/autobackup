#include "subjectmattermasteryhelper.h"

#include <algorithm>

#include <QFile>
#include <QTextStream>

//TODOoptional: keep track of the questions/answers success rate on an "overall" basis (answeredCorrectly divided by timesAsked)... and do something with it like present it at the end (reverse sorted so the ones missed the most are suggested to study even more) etc. Perhaps at the end we could ask if they want to take the test loop AGAIN but now ordered using that success rate instead of randomly
SubjectMatterMasteryHelper::SubjectMatterMasteryHelper(QObject *parent)
    : QObject(parent)
    , m_OwnIoDevice(false)
    , m_SubjectMatterQuestionsAndAnswersIterator(0)
{ }
SubjectMatterMasteryHelper::~SubjectMatterMasteryHelper()
{
    maybeCleanup();
    if(m_SubjectMatterQuestionsAndAnswersIterator)
        delete m_SubjectMatterQuestionsAndAnswersIterator;
}
void SubjectMatterMasteryHelper::readAllQuestionsFromSubjectMatterIoDeviceAndScrambleTheirOrdering()
{
    m_SubjectMatterQuestionsAndAnswers.clear();
    m_SubjectMatterQuestionsAndAnswersGotWrong.clear();
    QTextStream subjectMatterQuestionsAndAnswersTextStream(m_IoDeviceToSubjectMatter);
    while(!subjectMatterQuestionsAndAnswersTextStream.atEnd())
    {
        QString currentQuestion = subjectMatterQuestionsAndAnswersTextStream.readLine();
        if(currentQuestion.isEmpty())
            continue; //ignore empty lines
        QString currentAnswer = subjectMatterQuestionsAndAnswersTextStream.readLine().toLower(); //TODOoptional: allow case mandatory (for english majors' subject matter, etc)
        if(currentAnswer.isEmpty())
        {
            emit e("error: question without answer detected: '" + currentQuestion + "'");
            m_IoDeviceToSubjectMatter->close();
            //maybeCleanup();
            emit quitRequested();
            return;
        }
        m_SubjectMatterQuestionsAndAnswers.append(qMakePair(currentQuestion, currentAnswer));
    }
    m_IoDeviceToSubjectMatter->close();
    std::random_shuffle(m_SubjectMatterQuestionsAndAnswers.begin(), m_SubjectMatterQuestionsAndAnswers.end());
    m_CurrentSubjectMatterQuestionsAndAnswers = m_SubjectMatterQuestionsAndAnswers;
    if(m_SubjectMatterQuestionsAndAnswersIterator)
        delete m_SubjectMatterQuestionsAndAnswersIterator;
    m_SubjectMatterQuestionsAndAnswersIterator = new QListIterator<QuestionAndAnswerType>(m_CurrentSubjectMatterQuestionsAndAnswers);
}
void SubjectMatterMasteryHelper::askNextQuestionInSubjectMatterIoDevice()
{
    if(m_SubjectMatterQuestionsAndAnswersIterator->hasNext())
    {
        m_CurrentQuestionAndAnswer = m_SubjectMatterQuestionsAndAnswersIterator->next();
        emit questionAsked(m_CurrentQuestionAndAnswer.first);
    }
    else
    {
        //TODOreq: end of questions. see if any were wrong, shuffle those and re-present (<- a word where a hyphen makes a big difference)

        if(m_SubjectMatterQuestionsAndAnswersGotWrong.length() == 0)
        {
            //TODOreq: gratz, you got them all right

            //TODOreq: if they didn't get them all right on the very first try, re-present ALL of them again. it should loop FOREVER until they manage to get the entire test 100%
            emit testingFinished();
        }
        else //got at least one answer wrong
        {
            m_CurrentSubjectMatterQuestionsAndAnswers = m_SubjectMatterQuestionsAndAnswersGotWrong;
            m_SubjectMatterQuestionsAndAnswersGotWrong.clear();
            std::random_shuffle(m_CurrentSubjectMatterQuestionsAndAnswers.begin(), m_CurrentSubjectMatterQuestionsAndAnswers.end());
            delete m_SubjectMatterQuestionsAndAnswersIterator;
            m_SubjectMatterQuestionsAndAnswersIterator = new QListIterator<QuestionAndAnswerType>(m_CurrentSubjectMatterQuestionsAndAnswers);
            askNextQuestionInSubjectMatterIoDevice(); //TODOreq: infinite loop if no questions/answers in subject matter file
        }
    }
}
void SubjectMatterMasteryHelper::maybeCleanup()
{
    if(m_OwnIoDevice)
    {
        m_OwnIoDevice = false;
        delete m_IoDeviceToSubjectMatter;
    }
    m_IoDeviceToSubjectMatter = 0;
}
void SubjectMatterMasteryHelper::startSubjectMatterMasteryHelper(const QString &filenameOfSubjectMatter)
{
    m_OwnIoDevice = true; //TODOreq: set back to false at the end of session, if multiple sessions?
    QFile *subjectMatterFile = new QFile(filenameOfSubjectMatter, this);
    startSubjectMatterMasteryHelper(subjectMatterFile);
}
//io device should not be opened before passing
void SubjectMatterMasteryHelper::startSubjectMatterMasteryHelper(QIODevice *ioDeviceToSubjectMatter)
{
    m_IoDeviceToSubjectMatter = ioDeviceToSubjectMatter;
    if(!m_IoDeviceToSubjectMatter->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open subject matter for reading");
        //maybeCleanup();
        emit quitRequested();
        return;
    }
    readAllQuestionsFromSubjectMatterIoDeviceAndScrambleTheirOrdering(); //was going to do a more memory efficient version, but randomization warrants (not necessarily requires) loading the entire file in memory to KISS
    askNextQuestionInSubjectMatterIoDevice();
}
void SubjectMatterMasteryHelper::questionAnswered(const QString &answerAttempt)
{
    if(answerAttempt.toLower() == m_CurrentQuestionAndAnswer.second)
    {
        //answer correct
        emit questionAnsweredGraded(true, m_CurrentQuestionAndAnswer.second);
    }
    else
    {
        //answer incorrect
        m_SubjectMatterQuestionsAndAnswersGotWrong.append(m_CurrentQuestionAndAnswer);
        emit questionAnsweredGraded(false, m_CurrentQuestionAndAnswer.second);
    }
    askNextQuestionInSubjectMatterIoDevice(); //TODOoptional: allow multiple guesses before going on etc (for now, KISS)
}
