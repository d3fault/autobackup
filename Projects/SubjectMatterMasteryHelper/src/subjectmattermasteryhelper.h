#ifndef SUBJECTMATTERMASTERYHELPER_H
#define SUBJECTMATTERMASTERYHELPER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QScopedPointer>

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

    void readAllQuestionsFromSubjectMatterIoDeviceAndScrambleTheirOrdering();
    void askNextQuestionInSubjectMatterIoDevice();
    void maybeCleanup();
signals:
    void questionAsked(const QString &question);
    void questionAnsweredGraded(bool answerAttemptWasCorrect, const QString &answerIfAttemptWasIncorrect);
    void testingFinished();
    void e(const QString &);
    void quitRequested();
public slots:
    void startSubjectMatterMasteryHelper(const QString &filenameOfSubjectMatter);
    void startSubjectMatterMasteryHelper(QIODevice *ioDeviceToSubjectMatter);
    void questionAnswered(const QString &answerAttempt);
};

#endif // SUBJECTMATTERMASTERYHELPER_H
