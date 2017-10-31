#include "debutoutput.h"

#include <QDebug>

DebugOutput::DebugOutput(QObject *parent) :
    QObject(parent)
{ }
void DebugOutput::handleE(QString msg)
{
    qDebug() << msg;
}
