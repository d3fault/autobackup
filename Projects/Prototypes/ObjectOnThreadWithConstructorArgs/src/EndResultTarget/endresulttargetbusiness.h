#ifndef ENDRESULTTARGETBUSINESS_H
#define ENDRESULTTARGETBUSINESS_H

#include <QObject>

class EndResultTargetBusiness : public QObject
{
    Q_OBJECT
public:
    explicit EndResultTargetBusiness(int simpleArg1, QString implicitlySharedQString, QObject *parent = 0);
private:
    int m_SimpleArg1ToBeUsedByThisClass;
    QString m_ImplicitlySharedQStringToBeUsedByThisClass;
signals:
    
public slots:
    
};

#endif // ENDRESULTTARGETBUSINESS_H
