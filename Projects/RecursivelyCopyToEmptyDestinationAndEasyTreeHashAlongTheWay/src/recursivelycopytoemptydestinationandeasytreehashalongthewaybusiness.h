#ifndef RECURSIVELYCOPYTOEMPTYDESTINATIONANDEASYTREEHASHALONGTHEWAYBUSINESS_H
#define RECURSIVELYCOPYTOEMPTYDESTINATIONANDEASYTREEHASHALONGTHEWAYBUSINESS_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>

#include "easytreehasher.h"

//This Business class seems ridiculously irrelevant in this project... but at the same time I don't like the thought of my actual function call to do the recursive copy being [ONLY] a slot. At least with this Business class existing it leaves the possibility for me to change it to return a bool or some shit. But as it stands as of writing this, it's just forwarding function calls and therefore POINTLESS. Is Business in charge of sanitizing input [further]?... such as making sure the destination directory is empty? In EasyTreeGui I made the Business responsible for setting up the QIODevice (QFile) that the EasyTree works with... which means I can easily collect the EasyTree output into a QByteArray or transmit it over network etc instead of writing to a file (leaves options open).... but is that really the only benefit to having a Business? Should I do that in this project? As of this writing I am just passing in the EasyTreeHash output filename as a string lol (but don't have any of the body of the recursive copy written). WHO SHOULD SANITIZE THE USER INPUT (in this case just verify that destination is in fact empty)????? DOES IT EVEN MATTER? DOES IT REALLY HELP ALL THAT MUCH TO PASS IN AN ALREADY OPEN QIODEVICE INSTEAD OF A STRING FILENAME? Definitely more... portable... but does it fucking matter? What is a Business? What is the meaning of life?
//Fuck it just make it work, I want to complete + RUN this application twice within the next day oshi-
//Ok I just determined (decided, really) that the Business is going to "translate" the "user input" (strings) to data objects for the backend. So 2 QDirs and 1 QIODevice. Sounds good enough (the algorithm is data even at UI level fuggit (or should I say.. thank you QComboBox for your sexy integration with QVariant? (Now I am wondering what is the PROPER way to code... Data objects in the UI (QDirs/QIODevices? IDFK? Maybe it's just because Qt is... TOO good at what it does that it makes it easy to code with bad design (Data types in UI rofl))))
//Still dunno who should do validation. Makes more sense for data object to do it, seeing as it's the "portable" and "reusable" part (and the sanitation/validation would be used in all (????? yes, all, so long as EasyTreeHasher really is only ever used with an empty target directory (but a rewrite/redesign/different-project could change that!)) other occurances too)
class RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness : public QObject
{
    Q_OBJECT
public:
    explicit RecursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWayBusiness(QObject *parent = 0);
private:
    EasyTreeHasher m_EasyTreeHasher;
signals:
    void d(const QString &);
    void copyOperationComplete();
public slots:
    void recursivelyCopyToEmptyDestinationAndEasyTreeHashAlongTheWay(const QString &sourceDirectory, const QString &emptyDestinationDirectory, const QString &easyTreeHashOutputFilePath, QCryptographicHash::Algorithm algorithm);
};

#endif // RECURSIVELYCOPYTOEMPTYDESTINATIONANDEASYTREEHASHALONGTHEWAYBUSINESS_H
