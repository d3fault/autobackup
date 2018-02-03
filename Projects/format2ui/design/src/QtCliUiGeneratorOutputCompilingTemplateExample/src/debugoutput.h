#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include <QObject>

class QtCliUiGeneratorOutputCompilingTemplateExample;

class DebugOutput : public QObject
{
    Q_OBJECT
public:
    explicit DebugOutput(QtCliUiGeneratorOutputCompilingTemplateExample *qtCliUiGeneratorOutputCompilingTemplateExample, QObject *parent = 0);
private:
    QtCliUiGeneratorOutputCompilingTemplateExample *m_QtCliUiGeneratorOutputCompilingTemplateExample;
private slots:
    void handleFinishedCollectingUiVariables(bool success);
};

#endif // DEBUGOUTPUT_H
