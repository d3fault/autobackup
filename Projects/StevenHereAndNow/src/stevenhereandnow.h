#ifndef STEVENHEREANDNOW_H
#define STEVENHEREANDNOW_H

#include <QObject>
#include "bases/ievolvedmonkey.h"

class StevenHereAndNow : public QObject, public IEvolvedMonkey /*because the general dna is mostly the same (not to imply there isn't a huge difference between every single person). we are a clone function after all*/
{
    Q_OBJECT
public:
    explicit StevenHereAndNow(QObject *parent = 0);
protected:
    virtual ILawsOfPhysics *implementation();
signals:
    void o(const QString &msg);
public slots:
    void handleSayRequested(const QString &whatWasRequestedToBeSaid);
};

#endif // STEVENHEREANDNOW_H
