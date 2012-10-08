#include "easytreetest.h"

EasyTreeTest::EasyTreeTest(QObject *parent) :
    QObject(parent), m_EasyTree(0), m_MainWidget(0), m_ThreadBusiness(0)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()), Qt::DirectConnection);
}
void EasyTreeTest::start()
{
    if(!m_EasyTree)
    {
        m_EasyTree = new EasyTree();
        m_MainWidget = new mainWidget();
        m_MainWidget->setBusiness(m_EasyTree);
        m_MainWidget->show();
        m_ThreadBusiness = new QThread();
        m_EasyTree->moveToThread(m_ThreadBusiness);
        m_ThreadBusiness->start();

        QMetaObject::invokeMethod(m_EasyTree, "initialize", Qt::QueuedConnection);
    }
}
EasyTreeTest::~EasyTreeTest()
{
    delete m_MainWidget;
    delete m_EasyTree;
    delete m_ThreadBusiness;
}
void EasyTreeTest::handleAboutToQuit()
{
    m_ThreadBusiness->quit();
    m_ThreadBusiness->wait();
}
