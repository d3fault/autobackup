#ifndef QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
#define QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H

#include <QObject>

#include <QTextStream>

class QtCliUiGeneratorOutputCompilingTemplateExample : public QObject
{
    Q_OBJECT
public:
    explicit QtCliUiGeneratorOutputCompilingTemplateExample(QObject *parent = 0);
    void collectUiVariables();
private:
    QString query(const QString &queryString);

    QTextStream m_StdIn;
    QTextStream m_StdOut;
signals:
    void finishedCollectingUiVariables(const QString &firstName, const QString &lastName);
};

#endif // QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
