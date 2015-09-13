#include "subjectmattermasteryhelper.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>

//TODOoptional: keep track of the questions/answers success rate on an "overall" basis (answeredCorrectly divided by timesAsked)... and do something with it like present it at the end (reverse sorted so the ones missed the most are suggested to study even more) etc. Perhaps at the end we could ask if they want to take the test loop AGAIN but now ordered using that success rate instead of randomly
SubjectMatterMasteryHelper::SubjectMatterMasteryHelper(QObject *parent)
    : QObject(parent)
    , m_OwnIoDevice(false)
    , m_SubjectMatterQuestionsAndAnswersIterator(0)
    , m_AnswersTooLongUseMultipleChoice(false)
    , m_DontRandomize(false)
    , m_LoopUntilMastered(false)
    , m_TheyMissedAtLeastOneQuestion(false)

{
    qsrand(QDateTime::currentMSecsSinceEpoch());
}
SubjectMatterMasteryHelper::~SubjectMatterMasteryHelper()
{
    maybeCleanup();
    if(m_SubjectMatterQuestionsAndAnswersIterator)
        delete m_SubjectMatterQuestionsAndAnswersIterator;
}
void SubjectMatterMasteryHelper::readAllQuestionsFromSubjectMatterIoDeviceAndMaybeScrambleTheirOrdering()
{
    m_SubjectMatterQuestionsAndAnswers.clear();
    m_SubjectMatterQuestionsAndAnswersGotWrong.clear();
    m_AnswersDeduplicatedForMultipleChoiceShenanigans.clear();
    QTextStream subjectMatterQuestionsAndAnswersTextStream(m_IoDeviceToSubjectMatter);
    while(!subjectMatterQuestionsAndAnswersTextStream.atEnd())
    {
        QString currentQuestion = subjectMatterQuestionsAndAnswersTextStream.readLine();
        if(currentQuestion.isEmpty())
            continue; //ignore empty lines
        QString currentAnswer = subjectMatterQuestionsAndAnswersTextStream.readLine().toLower(); //TODOoptional: allow case sensitivity (for english majors' subject matter, etc)
        if(currentAnswer.isEmpty())
        {
            emit e("error: question without answer detected: '" + currentQuestion + "'");
            m_IoDeviceToSubjectMatter->close();
            //maybeCleanup();
            emit quitRequested();
            return;
        }
        m_SubjectMatterQuestionsAndAnswers.append(qMakePair(currentQuestion, currentAnswer));
        m_AnswersDeduplicatedForMultipleChoiceShenanigans.insert(currentAnswer);
    }
    m_IoDeviceToSubjectMatter->close();
    if(!m_DontRandomize)
        shuffleList(&m_SubjectMatterQuestionsAndAnswers);
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
            if(m_LoopUntilMastered && m_TheyMissedAtLeastOneQuestion)
            {
                m_TheyMissedAtLeastOneQuestion = false;
                emit o("\n -- Re-taking test because you missed at least one question -- \n\n");
                if(!m_DontRandomize)
                    shuffleList(&m_SubjectMatterQuestionsAndAnswers);
                m_CurrentSubjectMatterQuestionsAndAnswers = m_SubjectMatterQuestionsAndAnswers;
                delete m_SubjectMatterQuestionsAndAnswersIterator;
                m_SubjectMatterQuestionsAndAnswersIterator = new QListIterator<QuestionAndAnswerType>(m_CurrentSubjectMatterQuestionsAndAnswers);
                askNextQuestionInSubjectMatterIoDevice();
                return;
            }
            emit o("gratz, you got them all right\n");
            emit testingFinished();
        }
        else //got at least one answer wrong
        {
            m_CurrentSubjectMatterQuestionsAndAnswers = m_SubjectMatterQuestionsAndAnswersGotWrong;
            m_SubjectMatterQuestionsAndAnswersGotWrong.clear();
            if(!m_DontRandomize)
                shuffleList(&m_CurrentSubjectMatterQuestionsAndAnswers);
            //std::random_shuffle(m_CurrentSubjectMatterQuestionsAndAnswers.begin(), m_CurrentSubjectMatterQuestionsAndAnswers.end()); //TODOreq: random_shuffle is tits without c++11 support to get rand_device (and fuck c++11)... or maybe there's another way to seed? but yea i'm seeing the same patterns over and over -_-
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
void SubjectMatterMasteryHelper::startSubjectMatterMasteryHelper(const QString &filenameOfSubjectMatter, bool answersTooLongUseMultipleChoice, bool dontRandomize, bool loopUntilMastered)
{
    m_OwnIoDevice = true; //TODOreq: set back to false at the end of session, if multiple sessions?
    QFile *subjectMatterFile = new QFile(filenameOfSubjectMatter, this);
    startSubjectMatterMasteryHelper(subjectMatterFile, answersTooLongUseMultipleChoice, dontRandomize, loopUntilMastered);
}
//io device should not be opened before passing
void SubjectMatterMasteryHelper::startSubjectMatterMasteryHelper(QIODevice *ioDeviceToSubjectMatter, bool answersTooLongUseMultipleChoice, bool dontRandomize, bool loopUntilMastered)
{
    m_IoDeviceToSubjectMatter = ioDeviceToSubjectMatter;
    m_AnswersTooLongUseMultipleChoice = answersTooLongUseMultipleChoice;
    m_DontRandomize = dontRandomize;
    m_LoopUntilMastered = loopUntilMastered;
    m_TheyMissedAtLeastOneQuestion = false;
    if(!m_IoDeviceToSubjectMatter->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit e("failed to open subject matter for reading");
        //maybeCleanup();
        emit quitRequested();
        return;
    }
    readAllQuestionsFromSubjectMatterIoDeviceAndMaybeScrambleTheirOrdering(); //was going to do a more memory efficient version, but randomization warrants (not necessarily requires) loading the entire file in memory to KISS
    askNextQuestionInSubjectMatterIoDevice();
}
void SubjectMatterMasteryHelper::questionAnswered(const QString &answerAttempt)
{
    QString answerAttemptLowercase = answerAttempt.toLower();
    if(m_AnswersTooLongUseMultipleChoice) //hack'ish, this might turn into a spaghetti code app
    {
        if(answerAttemptLowercase.isEmpty()) //we wait until they enter an empty newline before showing the multiple choice selections (to give them time to guess it in their head)
        {
#define SubjectMatterMasteryHelper_NUM_MULTIPLE_CHOICE_ANSWERS 4
            QList<QString> multipleChoiceAnswers;
            multipleChoiceAnswers.append(m_CurrentQuestionAndAnswer.second);
            int numMultipleChoiceAnswers = qMin(m_AnswersDeduplicatedForMultipleChoiceShenanigans.size(), SubjectMatterMasteryHelper_NUM_MULTIPLE_CHOICE_ANSWERS);
            while(multipleChoiceAnswers.size() < numMultipleChoiceAnswers)
            {
                QString wrongAnswerCandidate;
                do
                {
                    wrongAnswerCandidate = m_AnswersDeduplicatedForMultipleChoiceShenanigans.toList().at(qrand() % m_AnswersDeduplicatedForMultipleChoiceShenanigans.size());
                }
                while(multipleChoiceAnswers.contains(wrongAnswerCandidate));
                multipleChoiceAnswers.append(wrongAnswerCandidate);
            }
            shuffleList(&multipleChoiceAnswers);
            QString multipleChoiceAnswersString; // = "\ta) " + multipleChoiceAnswers + "\n\tb) " + ;
            char x = 'a'; //TODOoptional: if I ever want to allow more than 26 multiple choice answers, I need to rethink this strategy
            m_CurrentMultipleChoiceLettersAndAnswers.clear();
            Q_FOREACH(QString currentMultipleChoiceAnswer, multipleChoiceAnswers)
            {
                m_CurrentMultipleChoiceLettersAndAnswers.insert(x, currentMultipleChoiceAnswer);
                multipleChoiceAnswersString.append("\t" + QString(QChar(x++)) + ") " + currentMultipleChoiceAnswer + "\n");
            }
            emit o(multipleChoiceAnswersString); //TODOreq: a gui version of this app would probably need a signal more specific than 'o'
            return;
        }
        else //they entered a multiple choice selection
        {
            QString longAnswerSignaledThroughMultipleChoiceAnswer_Lowercase = m_CurrentMultipleChoiceLettersAndAnswers.value(answerAttemptLowercase.at(0).toLatin1());
            if(answerAttemptLowercase.length() != 1 || longAnswerSignaledThroughMultipleChoiceAnswer_Lowercase.isEmpty())
            {
                emit e("invalid multiple choice selection");
                return;
            }
            answerAttemptLowercase = longAnswerSignaledThroughMultipleChoiceAnswer_Lowercase; //fall out of scope and resume grading answer like normal
        }
    }


    if(answerAttemptLowercase.isEmpty())
        return; //ignore accidental enter presses [when not in multiple choice mode]
    if(answerAttemptLowercase == m_CurrentQuestionAndAnswer.second)
    {
        //answer correct
        emit questionAnsweredGraded(true, m_CurrentQuestionAndAnswer.second);
    }
    else
    {
        //answer incorrect
        m_TheyMissedAtLeastOneQuestion = true;
        m_SubjectMatterQuestionsAndAnswersGotWrong.append(m_CurrentQuestionAndAnswer);
        emit questionAnsweredGraded(false, m_CurrentQuestionAndAnswer.second);
    }
    askNextQuestionInSubjectMatterIoDevice(); //TODOoptional: allow multiple guesses before going on etc (for now, KISS)
}
