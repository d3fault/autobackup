#ifndef CLEANTHREADINGBACKEND1_H
#define CLEANTHREADINGBACKEND1_H

#include <QObject>
#include <QCryptographicHash>

class CleanThreadingBackend1 : public QObject
{
    Q_OBJECT
public:
    explicit CleanThreadingBackend1(QObject *parent = 0);
private:
    QCryptographicHash m_Hasher;
signals:
    void hashGenerated(const QString &hash);
public slots:
    void thrashHashStringNtimes(const QString &stringToThrashHash, int numTimesToHash);
};

#endif // CLEANTHREADINGBACKEND1_H
