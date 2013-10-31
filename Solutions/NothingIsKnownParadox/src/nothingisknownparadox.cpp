#include "nothingisknownparadox.h"

#include <iostream>

NothingIsKnownParadox::NothingIsKnownParadox(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), &n, SLOT(handleAboutToQuit()));

    connect(&m_BrainThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBrainReadyForConnections()));
    m_BrainThread.start();
}
void NothingIsKnownParadox::userInput()
{
    std::cout << "-Brain-" << std::endl;
    std::cout << "\t1 - Determine Whether Or Not Nothing Is Known" << std::endl;
    std::cout << "\t2 - Exit (or: CTRL+C)" << std::cout;
    quint8 selection;
    std::cin >> selection;
    if(selection == 1)
    {
        std::cout << "Beginning Trying To Determine Whether Or Not Nothing Is Known (press CTRL+C to stop)..." << std::endl;
        emit wantToKnowWhetherOrNotNothingIsKnown();
    }
    else
    {
        handleAboutToQuit();
    }
}
void NothingIsKnownParadox::handleBrainReadyForConnections()
{
    Brain *brain = m_BrainThread.getObjectPointerForConnectionsOnly();
    connect(brain, SIGNAL(reportWhetherOrNotSomethingIsKnown(bool)), this, SLOT(handleBrainReportingWhetherOrNotNothingIsKnown(bool)));
    connect(this, SIGNAL(wantToKnowWhetherOrNotNothingIsKnown()), brain, SLOT(determineWhetherOrNotNothingIsKnown()));

    userInput();
}
void NothingIsKnownParadox::handleAboutToQuit()
{
    std::cout << "Stopping...";

    m_BrainThread.quit();
    m_BrainThread.wait();

    std::cout << "Stopped" << std::endl;
}
void NothingIsKnownParadox::handleBrainReportingWhetherOrNotNothingIsKnown(bool nothingIsKnown)
{
    if(!nothingIsKnown)
    {
        std::cout << "Something Is Known" << std::endl;
    }
    else
    {
        std::cout << "Nothing Is Known" << std::endl;
    }
}
