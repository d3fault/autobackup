#include "easytreehashdiffanalyzerbusiness.h"

EasyTreeHashDiffAnalyzerBusiness::EasyTreeHashDiffAnalyzerBusiness(QObject *parent) :
    QObject(parent)
{
    connect(&m_EasyTreeHashDiffAnalyzer, SIGNAL(d(QString)), this, SIGNAL(d(QString)));
}
void EasyTreeHashDiffAnalyzerBusiness::diffAndAnalyze(const QString &input1, const QString &input2)
{
    if(!QFile::exists(input1))
    {
        emit d("Input 1 Does Not Exist");
        return;
    }
    if(!QFile::exists(input2))
    {
        emit d("Input 2 Does Not Exist");
        return;
    }
    QFile input1File(input1);
    QFile input2File(input2);
    m_EasyTreeHashDiffAnalyzer.analyzeEasyTreeHashesAndReportDifferences(&input1File, &input2File);
}
