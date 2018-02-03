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
    QString firstName() const;
    QString lastName() const;
private:
    QString query(const QString &queryString);

    QTextStream m_StdIn;
    QTextStream m_StdOut;

    QString m_FirstName;
    QString m_LastName;
signals:
    void finishedCollectingUiVariables(bool success);
};

#endif // QTCLIUIGENERATOROUTPUTCOMPILINGTEMPLATEEXAMPLE_H
