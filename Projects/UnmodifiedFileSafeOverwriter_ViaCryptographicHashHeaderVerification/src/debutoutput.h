#ifndef DEBUTOUTPUT_H
#define DEBUTOUTPUT_H

#include <QObject>

class DebugOutput : public QObject
{
    Q_OBJECT
public:
    explicit DebugOutput(QObject *parent = 0);

signals:

public slots:
    void handleE(QString msg);
};

#endif // DEBUTOUTPUT_H
