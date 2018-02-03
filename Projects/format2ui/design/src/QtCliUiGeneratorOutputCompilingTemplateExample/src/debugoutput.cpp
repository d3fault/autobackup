#include "debugoutput.h"

#include <QCoreApplication>
#include <QDebug>

#include "qtcliuigeneratoroutputcompilingtemplateexample.h"

DebugOutput::DebugOutput(QtCliUiGeneratorOutputCompilingTemplateExample *qtCliUiGeneratorOutputCompilingTemplateExample, QObject *parent) :
    QObject(parent)
  , m_QtCliUiGeneratorOutputCompilingTemplateExample(qtCliUiGeneratorOutputCompilingTemplateExample)
{
    connect(qtCliUiGeneratorOutputCompilingTemplateExample, &QtCliUiGeneratorOutputCompilingTemplateExample::finishedCollectingUiVariables, this, &DebugOutput::handleFinishedCollectingUiVariables);
}
void DebugOutput::handleFinishedCollectingUiVariables(bool success)
{
    qDebug(success ? "success" : "unsuccessful");
    if(success)
    {
        qDebug() << m_QtCliUiGeneratorOutputCompilingTemplateExample->firstName();
        qDebug() << m_QtCliUiGeneratorOutputCompilingTemplateExample->lastName();
    }
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
