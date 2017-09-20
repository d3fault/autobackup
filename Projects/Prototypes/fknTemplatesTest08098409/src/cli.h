#ifndef CLI_H
#define CLI_H

#include <QObject>

class Cli : public QObject
{
    Q_OBJECT
public:
    explicit Cli(QObject *parent = 0);
signals:
    void doFuckRequested();
};

#endif // CLI_H
