#include "recursivevideolengthandsizecalculatorgui.h"

RecursiveVideoLengthAndSizeCalculatorGui::RecursiveVideoLengthAndSizeCalculatorGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleRecursiveVideoLengthAndSizeCalculatorReadyForConnections()));
    m_BusinessThread.start();
}
void RecursiveVideoLengthAndSizeCalculatorGui::handleRecursiveVideoLengthAndSizeCalculatorReadyForConnections()
{
    RecursiveVideoLengthAndSizeCalculator *business = m_BusinessThread.getObjectPointerForConnectionsOnly();

    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(recursivelyCalculateVideoLengthsAndSizesRequested(QString)), business, SLOT(recursivelyCalculateVideoLengthsAndSizes(QString)));

    m_Gui.show();
}
void RecursiveVideoLengthAndSizeCalculatorGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
