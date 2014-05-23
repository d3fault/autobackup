#include "dasbuttoncli.h"

#include <QCoreApplication>

#include "dasbutton.h"

DasButtonCli::DasButtonCli(QObject *parent)
    : QObject(parent)
    , m_DasButton(new DasButton(this))
    , m_StdOut(stdout)
    , m_StdErr(stderr)
{
    //TODOoptional: parse args zzzzz
    QString prefixAkaHomeDir;
    QString filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater; //DasButton doesn't prepend it actually, but meh fuck it

    connect(m_DasButton, SIGNAL(o(QString)), this, SLOT(handleO(QString)));
    connect(m_DasButton, SIGNAL(e(QString)), this, SLOT(handleE(QString)));
    connect(m_DasButton, SIGNAL(pressFinished(bool)), this, SLOT(handlePressFinished(bool)));
    QMetaObject::invokeMethod(m_DasButton, "press", Q_ARG(QString, prefixAkaHomeDir), Q_ARG(QString, filepathOfCopyrightHeaderToBothUnprependAndPrependAgainLater));
}
void DasButtonCli::handlePressFinished(bool pressWasSuccessful)
{
    if(pressWasSuccessful)
    {
        handleO("press of das button finished successfully");
    }
    else
    {
        handleE("press of das button failed");
    }
    QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
}
void DasButtonCli::handleO(const QString &msg)
{
    m_StdOut << msg << endl;
}
void DasButtonCli::handleE(const QString &msg)
{
    m_StdErr << msg << endl;
}
