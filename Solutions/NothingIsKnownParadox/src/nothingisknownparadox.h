#ifndef NOTHINGISKNOWNPARADOX_H
#define NOTHINGISKNOWNPARADOX_H

#include <QObject>

class NothingIsKnownParadox : public QObject
{
    Q_OBJECT
public:
    explicit NothingIsKnownParadox(QObject *parent = 0);
signals:
    void o(const QString &msg);
};

#endif // NOTHINGISKNOWNPARADOX_H
