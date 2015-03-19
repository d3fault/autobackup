#ifndef BRAIN_H
#define BRAIN_H

#include <QObject>
#include <QStringList>

#define NOTHING_IS_KNOWN_CLAIM "Nothing is known"

class Brain : public QObject
{
    Q_OBJECT
public:
    Brain(QObject *parent = 0);
private:
    QStringList m_ThingsKnown;

    bool isTrue(const QString &claimBeingAnalyzed);
    void makeClaim(const QString &theClaim);
    void addKnownThing(const QString &thingKnown);
    void removeKnownThing(const QString &thingThatUsedToBeConsideredKnownButIsNowKnownToBeFalse);
signals:
    void o(const QString &msg);
    void whatIsThisQuestioned();
    void askSelfWhatIsKnown();
    void newInformationLearned();
    void oldInformationTurnedOutToBeFalse();
private slots:
    void realizeSomethingIsHappening();
    void thinkDeepAboutWhatThisIs();
    void evaluateKnownThings();
};

#endif // BRAIN_H
