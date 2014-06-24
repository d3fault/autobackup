#ifndef SUBJECTMATTERMASTERYHELPERCLI_H
#define SUBJECTMATTERMASTERYHELPERCLI_H

#include <QObject>
#include <QTextStream>

class SubjectMatterMasteryHelper;
class StandardInputNotifier;

class SubjectMatterMasteryHelperCli : public QObject
{
    Q_OBJECT
public:
    explicit SubjectMatterMasteryHelperCli(QObject *parent = 0);
private:
    SubjectMatterMasteryHelper *m_SubjectMatterMasteryHelper;
    StandardInputNotifier *m_StandardInputNotifier;
    QTextStream m_StdOut;
    QTextStream m_StdErr;

    void usage();
signals:
    void answeredQuestion(const QString &answerAttempt);
private slots:
    void handleQuestionAsked(const QString &question);
    void handleStandardInputReceivedLine(const QString &standardInputLine);
    void handleQuestionAnsweredGraded(bool answerAttemptWasCorrect, const QString &answerIfAttemptWasIncorrect);
    void handleE(const QString &msg);
    void handleQuitRequested();
};

#endif // SUBJECTMATTERMASTERYHELPERCLI_H
