#include "mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartButton = new QPushButton("Run Tests");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartButton);
    this->setLayout(m_Layout);


    m_TestDriver = new TestDriver();
    m_TestDriverThread = new QThread();
    m_TestDriver->moveToThread(m_TestDriverThread);
    m_TestDriverThread->start();

    connect(m_StartButton, SIGNAL(clicked()), m_TestDriver, SLOT(runTests()));
    connect(m_TestDriver, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));

    QMetaObject::invokeMethod(m_TestDriver, "init", Qt::QueuedConnection);
}

mainWidget::~mainWidget()
{
    delete m_TestDriver;
}
