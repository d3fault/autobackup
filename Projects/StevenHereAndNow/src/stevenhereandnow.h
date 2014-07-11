#ifndef STEVENHEREANDNOW_H
#define STEVENHEREANDNOW_H

#include <QObject>
#include "bases/ievolvedmonkey.h"

class StevenHereAndNow : public QObject, public IEvolvedMonkey
{
    Q_OBJECT
public:
    explicit StevenHereAndNow(QObject *parent = 0);
protected:
    virtual ILawsOfPhysics *implementation();
private:
    //QList<ILawsOfPhysics*> m_Files;
    TagCloud<ILawsOfPhysics*> m_Files;
signals:
    //void encryptedFileAdded(ILawsOfPhysics *encryptedFile);
    void fileAdded(ILawsOfPhysics *file);
    void o(const QString &msg);
public slots:
    //void addFileEncrypted(ILawsOfPhysics *decryptedInputFile);
    void addFile(ILawsOfPhysics *inputFile);
    void handleSayRequested(const QString &whatWasRequestedToBeSaid);
};

#endif // STEVENHEREANDNOW_H
