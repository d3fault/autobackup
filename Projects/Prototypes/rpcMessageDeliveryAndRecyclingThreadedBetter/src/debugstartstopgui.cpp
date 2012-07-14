#include "debugstartstopgui.h"

DebugStartStopGui::DebugStartStopGui(IDebuggableStartableStoppableBackend *debuggableStartableStoppableBackend)
{
    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_StartButton = new QPushButton("Start");
    m_StopButton = new QPushButton("Stop");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_StartButton);
    m_Layout->addWidget(m_StopButton);


    //QVBoxLayout *layout = static_cast<QVBoxLayout*>(this->layout());
    QPushButton *createBankAccountSimButton = new QPushButton("Create Bank Account");
    m_Layout->addWidget(createBankAccountSimButton);

    //this->layout()->addWidget(&createBankAccountSimButton);

    connect(createBankAccountSimButton, SIGNAL(clicked()), debuggableStartableStoppableBackend, SLOT(simulateCreateBankAccount()));


    this->setLayout(m_Layout);

    m_BackendThread = new QThread();
    debuggableStartableStoppableBackend->moveToThread(m_BackendThread);
    m_BackendThread->start();

    connect(debuggableStartableStoppableBackend, SIGNAL(d(QString)), this, SLOT(handleD(QString)));
    connect(m_StartButton, SIGNAL(clicked()), debuggableStartableStoppableBackend, SLOT(start()));
    connect(m_StopButton, SIGNAL(clicked()), debuggableStartableStoppableBackend, SLOT(stop()));

    QMetaObject::invokeMethod(debuggableStartableStoppableBackend, "init", Qt::QueuedConnection);
}
void DebugStartStopGui::handleD(const QString &text)
{
    m_Debug->appendPlainText(text);
}
