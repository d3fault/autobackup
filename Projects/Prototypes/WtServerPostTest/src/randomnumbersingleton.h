#ifndef RANDOMNUMBERSINGLETON_H
#define RANDOMNUMBERSINGLETON_H

#include <QObject>
#include <QThread>
#include <QTimer>

class RandomNumberSingleton : public QObject
{
    Q_OBJECT
public:
    static RandomNumberSingleton* Instance();
private:
    static RandomNumberSingleton *m_pInstance;
    RandomNumberSingleton();
    QThread *m_MyThread;
    QTimer *m_GenerateNumberTimer;
private slots:
    void init();
    void generateNumber();
signals:
    void numberGenerated(int number);
};

#endif // RANDOMNUMBERSINGLETON_H
