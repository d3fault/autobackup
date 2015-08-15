#ifndef SUBJECTMATTERMASTERYHELPER_H
#define SUBJECTMATTERMASTERYHELPER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QScopedPointer>
#include <QMap>
#include <QSet>

class QIODevice;

typedef QPair<QString /*question*/, QString /*answer*/> QuestionAndAnswerType;

class SubjectMatterMasteryHelper : public QObject
{
    Q_OBJECT
public:
    explicit SubjectMatterMasteryHelper(QObject *parent = 0);
    ~SubjectMatterMasteryHelper();
private:
    QIODevice *m_IoDeviceToSubjectMatter;
    bool m_OwnIoDevice;
    QList<QuestionAndAnswerType> m_SubjectMatterQuestionsAndAnswers;
    QList<QuestionAndAnswerType> m_CurrentSubjectMatterQuestionsAndAnswers; //shrinks as they get answers right
    QListIterator<QuestionAndAnswerType> *m_SubjectMatterQuestionsAndAnswersIterator;
    QuestionAndAnswerType m_CurrentQuestionAndAnswer;
    QList<QuestionAndAnswerType> m_SubjectMatterQuestionsAndAnswersGotWrong;
    bool m_AnswersTooLongUseMultipleChoice;
    bool m_DontRandomize;
    QMap<char, QString> m_CurrentMultipleChoiceLettersAndAnswers;
    QSet<QString> m_AnswersDeduplicatedForMultipleChoiceShenanigans;
    bool m_LoopUntilMastered;
    bool m_TheyMissedAtLeastOneQuestion;

    void readAllQuestionsFromSubjectMatterIoDeviceAndMaybeScrambleTheirOrdering();
    template <class T>
    void shuffleList(QList<T> *theList)
    {
        QList<T> shuffled;
        while(!theList->isEmpty())
        {
            shuffled.append(theList->takeAt(qrand() % theList->size()));
        }
        *theList = shuffled;
    }
    void askNextQuestionInSubjectMatterIoDevice();
    void maybeCleanup();
signals:
    void questionAsked(const QString &question);
    void questionAnsweredGraded(bool answerAttemptWasCorrect, const QString &answerIfAttemptWasIncorrect);
    void testingFinished();
    void o(const QString &);
    void e(const QString &);
    void quitRequested();
public slots:
    void startSubjectMatterMasteryHelper(const QString &filenameOfSubjectMatter, bool answersTooLongUseMultipleChoice = false, bool dontRandomize = false, bool loopUntilMastered = false);
    void startSubjectMatterMasteryHelper(QIODevice *ioDeviceToSubjectMatter, bool answersTooLongUseMultipleChoice = false, bool dontRandomize = false, bool loopUntilMastered = false);
    void questionAnswered(const QString &answerAttempt);
};

#endif // SUBJECTMATTERMASTERYHELPER_H
