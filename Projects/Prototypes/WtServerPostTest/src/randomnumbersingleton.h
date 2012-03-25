#ifndef RANDOMNUMBERSINGLETON_H
#define RANDOMNUMBERSINGLETON_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>

class RandomNumberSingleton : public QObject
{
    Q_OBJECT
public:
    static RandomNumberSingleton* Instance();
private:
    static RandomNumberSingleton *m_pInstance;
    RandomNumberSingleton();
    static QThread *m_MyThread;
    QTimer *m_GenerateNumberTimer;
private Q_SLOTS:
    void init();
    void generateNumber();
Q_SIGNALS:
    void numberGenerated(int number);
};

#endif // RANDOMNUMBERSINGLETON_H
