#ifndef STEVENHEREANDNOW_H
#define STEVENHEREANDNOW_H

#include <QObject>

class StevenHereAndNow : public QObject, combined_implement(vicki, curt)
{
    Q_OBJECT
public:
    explicit StevenHereAndNow(QObject *parent = 0);
};

#endif // STEVENHEREANDNOW_H
