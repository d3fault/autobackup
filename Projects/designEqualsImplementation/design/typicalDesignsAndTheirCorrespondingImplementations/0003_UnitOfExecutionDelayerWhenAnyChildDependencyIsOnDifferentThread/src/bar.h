#ifndef BAR_H
#define BAR_H

#include <QObject>

class Bar : public QObject
{
    Q_OBJECT
public:
    explicit Bar(QObject *parent = 0);
signals:
    void barSignal(bool success);
public slots:
    void barSlot(const QString &cunt);
};

#endif // BAR_H
