#ifndef EASYTREETOTALSIZECALCULATOR_H
#define EASYTREETOTALSIZECALCULATOR_H

#include <QObject>
#include <QFile>
#include <QIODevice>
#include <QListIterator>

#include "easytreeparser.h"
#include "easytreehashitem.h"

class EasyTreeTotalSizeCalculator : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeTotalSizeCalculator(QObject *parent = 0);    
signals:
    void d(const QString &);
public slots:
    void calculateTotalSizeOfEasyTreeEntries(const QString &filepathOfEasyTreeFile);
    void calculateTotalSizeOfEasyTreeEntries(QIODevice *ioDeviceOfEasyTreeFile);
};

#endif // EASYTREETOTALSIZECALCULATOR_H
