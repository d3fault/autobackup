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

    bool isTrue(const QString &logic);
    //bool somethingCanBeDeterminedFromKnownThings();
signals:
    void o(const QString &msg);
    void whatIsThisQuestioned();
    void askSelfWhatIsKnown();
    void claimMade(const QString &theClaim);
    void addKnownThingRequested(const QString &thingKnown);
    void newInformationLearned();
    void removeKnownThingRequested(const QString &thingThatUsedToBeConsideredKnownButIsNowKnownToBeFalse);
    void oldInformationTurnedOutToBeFalse();
public slots:
    //void observeEnvironment();
private slots:
    void thinkDeepAboutWhatThisIs();
    void evaluateKnownThings();
    void makeClaim(const QString &theClaim);
    void addKnownThing(const QString &thingKnown);
    void removeKnownThing(const QString &thingThatUsedToBeConsideredKnownButIsNowKnownToBeFalse);
};

#endif // BRAIN_H
