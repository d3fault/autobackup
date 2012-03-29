#ifndef MYRANDOMNUMBERGENERATOR_H
#define MYRANDOMNUMBERGENERATOR_H

#include <QObject>
#include <QTimer>

class MyRandomNumberGenerator : public QObject
{
    Q_OBJECT
public:
    explicit MyRandomNumberGenerator(QObject *parent = 0);
private:
    QTimer *m_Timer;
signals:
    void randomNumberGenerated(int number);
public slots:
    void start();
    void stop();
private slots:
    void generateArandomNumber();
};

#endif // MYRANDOMNUMBERGENERATOR_H
