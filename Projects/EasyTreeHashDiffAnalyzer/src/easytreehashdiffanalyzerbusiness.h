#ifndef EASYTREEHASHDIFFANALYZERBUSINESS_H
#define EASYTREEHASHDIFFANALYZERBUSINESS_H

#include <QObject>
#include <QFile>

#include "easytreehashdiffanalyzer.h"

class EasyTreeHashDiffAnalyzerBusiness : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeHashDiffAnalyzerBusiness(QObject *parent = 0);
private:
    EasyTreeHashDiffAnalyzer m_EasyTreeHashDiffAnalyzer;
signals:
    void d(const QString &);
public slots:
    void diffAndAnalyze(const QString &input1, const QString &input2);
};

#endif // EASYTREEHASHDIFFANALYZERBUSINESS_H
