#include "gitunrollrerollcensorshipmachinegui.h"

//For some reason debugging/running in qt creator doesn't work on my test box for CLI apps... so eh that's why I'm making this GUI. That and so I can valgrind it (good practice)
//The simplicity and repetetiveness of this (cli <--> gui, lib; 'ARGUMENTS' = 'FIELDS') all makes me want to make a project generator app xD, but perhaps I should master submodules/etc first? OH WAIT I SHOULD LAUNCH FIRST (BUT IT'S SO FUN!!!!). I've seriously got it down to character by character perfect matching. I need a fracking template at the very least xD... I type the same shit far too often... BUT EVERY TIME I ATTEMPT IT, I GET SO CARRIED AWAY WITH HOW AWESOME I COULD MAKE IT (designEqualsImplementation) THAT I NEVER MAKE EVEN A SIMPLE ASS TEMPLATE. "Hmm I want to make a template building app but I want to first make a template for use in making the template building app"... pretty much sums it up. Being a coder can be a bitch sometimes, choosing when/where to optimize is like making me choose different flavors of equally tasty candy: CAN'T I HAVE BOTH? GAH GIMMEH INFINITE TIME PLX!
GitUnrollRerollCensorshipMachineGui::GitUnrollRerollCensorshipMachineGui(QObject *parent) :
    QObject(parent)
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(handleAboutToQuit()));

    connect(&m_BusinessThread, SIGNAL(objectIsReadyForConnectionsOnly()), this, SLOT(handleBusinessReadyForConnections()));
    m_BusinessThread.start();
}
void GitUnrollRerollCensorshipMachineGui::handleBusinessReadyForConnections()
{
    GitUnrollRerollCensorshipMachine *business = m_BusinessThread.getObjectPointerForConnectionsOnly();

    connect(business, SIGNAL(d(QString)), &m_Gui, SLOT(handleD(QString)));
    connect(&m_Gui, SIGNAL(unrollRerollGitRepoCensoringAtEachCommitRequested(QString,QString,QString,QString)), business, SLOT(unrollRerollGitRepoCensoringAtEachCommit(QString,QString,QString,QString)));

    m_Gui.show();
}
void GitUnrollRerollCensorshipMachineGui::handleAboutToQuit()
{
    m_BusinessThread.quit();
    m_BusinessThread.wait();
}
