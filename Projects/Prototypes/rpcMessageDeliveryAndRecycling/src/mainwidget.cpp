#include "mainwidget.h"

#include "messageDispensers/broadcasts/pendingbalanceaddeddetectedmessagedispenser.h" //include only needed for debugging/testing

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)
{
    //set up objects. we do this all at once so we ensure that we 'push' our broadcastDispensers from the main thread (the instantiator and current thread affinity) onto the thread it needs to be on (business impl just forwards the 'push' to whoever. only a bitcoin object/thread atm, but other broadcasts will have different objects/threads)

    //wait hold up, i'm not setting up recycling... i'm setting up destination object. i don't think there was a problem with this. fuck wtf am i doing.
    //STILL, it _IS_ the broadcast dispensers that i am needing to deal with. that i am needing to 'push' asep.. before all my .moveToThread calls.

    m_Business = new RpcServerBusinessImpl(); //RpcClientsHelper attaches to it's signals, which is why we pass it in
    m_ClientsHelper = new RpcClientsHelperAndDeliveryAcceptorAndNetwork(m_Business); //RpcClientsHelper attaches it's signals and sets up itself as the broadcasts' destination object
    m_Business->pushBroadcastDispensersToAppropriateBusinessThreads(); //broadcastDispenser has to be set up before this call, but we own it at this point (why there's no arg). T O D O r e q (nvm decided to make it pure virtual instead. good enough): this call could also be my check (in 'init' later) to make sure dispensers are moved appropriately. i mean it'd still require the user/developer to know to do it right... but still, this will raise attention


    TODO LEFT OFF -- compiling aside from this line.. but very very broken. create bank account messages are recycling on the bitcoin thread? wtf? might even start the below comments in a new project... scared this one might be too fucked.
    //perhaps something along the line of:
    //m_ClientsHelper->electBroadcastDispenserOwnerForPendingBalanceAddedMessages(m_Business->bitcoin()); //TODOreq: additionally, in this scenario, it might also be smart to make Bitcoin's constructor NOT accept a QObject *parent... so we don't accidentally pass in the business as a parent... which would cause bitcoin to move it's thread too.... no bueno (but also might not matter, since we'd just override it later?)

    //etc for each broadcast message type

    //then, when initializing m_ClientsHelper later... or starting it or whatever, we can just do simple boolean checks to make sure that each broadcast dispenser has an owner elected. if it doesn't, it spits out the appropriate error message and stops.
    //this at least forces the user/developer to elect an owner for every broadcast dispenser.
    //we should make note somewhere that it should be done BEFORE the new broadcast dispenser owner is .moveToThread'd (so we are a 'child' when it happens)
    //we could just check that the dispensers aren't 0 (i'd have to init them to zero, no biggy) instead of using booleans... since we have to send in the elected owner as the parent in the constructor (default of parent = 0 is not there)

    //also, still don't know if i need to do MessageDispenser::moveToThread(...); or if just setting it as a child really is enough. if the doc says it is, i guess that's enough to go by for the next prototype. but i won't be happy until i see threadIds matching in debug code~

    //during the 'elect' shit...right after the "new MessageDispenser(electedOwner)" we have a great opportunity to set the destination object for the specific dispenser to 'this'. much more organized than how i'm doing it now. shit's all over the place.

    m_BusinessThread = new QThread();
    m_Business->moveToThread(m_BusinessThread);
    m_BusinessThread->start();

    m_NetworkThread = new QThread();    
    m_ClientsHelper->moveToThread(m_NetworkThread);
    m_NetworkThread->start();

    m_Layout = new QVBoxLayout();
    m_Debug = new QPlainTextEdit();
    m_SimulateActionButton = new QPushButton("simulate create bank account");

    m_Layout->addWidget(m_Debug);
    m_Layout->addWidget(m_SimulateActionButton);
    this->setLayout(m_Layout);


    connect(m_ClientsHelper, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));
    connect(m_Business, SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString)));

    connect(m_Business->broadcastDispensers()->pendingBalanceAddedDetectedMessageDispenser(), SIGNAL(d(QString)), m_Debug, SLOT(appendPlainText(QString))); //we can hackily connect to this dispenser's d() here, but we need to go inside ClientsHelper to hook up createBankAccount dispenser's d()

    connect(m_SimulateActionButton, SIGNAL(clicked()), m_ClientsHelper, SLOT(handleSimulateActionButtonClicked()));

    connect(m_ClientsHelper, SIGNAL(initialized()), m_Business, SLOT(init()));

    QMetaObject::invokeMethod(m_ClientsHelper, "init", Qt::QueuedConnection);
}

mainWidget::~mainWidget()
{

}
