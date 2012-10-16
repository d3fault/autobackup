#ifndef CLEANOBSERVERPATTERNEXAMPLEBACKENDA_H
#define CLEANOBSERVERPATTERNEXAMPLEBACKENDA_H

#include <QObject>
#include <QCryptographicHash>

class CleanObserverPatternExampleBackendA : public QObject
{
    Q_OBJECT
public:
    explicit CleanObserverPatternExampleBackendA(QObject *parent = 0);
private:
    QCryptographicHash *m_Hasher;
signals:
    void hashGenerated(const QString &hash);
public slots:
    void threadStartingUp();
    void thrashHashStringNtimes(const QString &stringToThrashHash, int numTimesToHash);
    void threadShuttingDown();
};

#endif // CLEANOBSERVERPATTERNEXAMPLEBACKENDA_H
