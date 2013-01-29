#ifndef WORKINGINPUTTESTBUSINESS_H
#define WORKINGINPUTTESTBUSINESS_H

#include <QObject>

class WorkingInputTestBusiness : public QObject
{
    Q_OBJECT
public:
    explicit WorkingInputTestBusiness(int simpleIntArg1, QString implicitlySharedQStringArg2, QObject *parent = 0);
private:
    int m_SimpleIntArg1;
    QString m_ImplicitlySharedQStringArg2;
signals:
    
public slots:
    
};

#endif // WORKINGINPUTTESTBUSINESS_H
