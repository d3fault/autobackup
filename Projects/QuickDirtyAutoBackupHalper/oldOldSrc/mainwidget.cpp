#include "mainwidget.h"

//TODOopt: make "autobackup", the X (2 for now) truecrypt devices (opt: file containers!), and the place where dir.structure.txt is GENERATED (before copied over to autobackup dir for git add . and git commit) (going to use /ram/ as tmpfs methinks <3 fast-as-fuck) ------ all of them CUSTOMIZABLE ----- which also means I should save the settings in between application sessions. I think QSettings is used for that?
//^^^oh and i guess the passwords (or password files in my case) should be customizeable too...

//lol at how long this 'quickdirty' app is taking.... fml

mainWidget::mainWidget(QuickDirtyAutoBackupHalper *business, QWidget *parent)
    : QWidget(parent), m_Business(business), m_Mounted(false), m_AboutToCommitWaitingForMount(false), m_Committing(false), m_AboutToShutdownWaitingForDismount(false)
{
    QMetaObject::invokeMethod(this, "initialize", Qt::QueuedConnection);
}
mainWidget::~mainWidget()
{
    delete m_BusinessThread;
}
void mainWidget::probeMountStatus()
{
    //TODO: figure out if the user has already mounted the volumes in the GUI. do this by using truecrypt -l for example

    //TODOreq: if one is mounted and the other isn't, do something to make it so the app does not function at all (after spitting out an appropriate error message). one thing you could do is make all the connect()'ions fail. so i could add a if(!fucked) in the rigConnections call etc
}
void mainWidget::updateGuiBasedOnMountedOrDismounted()
{
    m_MountTcVolumesButton->setEnabled(!m_Mounted);
    m_DismountTcVolumesButton->setEnabled(m_Mounted);

    m_MountAsReadOnly->setEnabled(!m_Mounted);
}
void mainWidget::continueCommitProcessBecauseMounted()
{
    QString commitMsg = m_GitCommitMessage->text();
    if(commitMsg.trimmed() == QString(AUTO_COMMIT_DATE_REPLACE_SPECIAL_TXT))
    {
        commitMsg = (QString("Auto-Commit2 @") + QDateTime::currentDateTime().toString());
    }
    emit commitRequested(commitMsg);
}
void mainWidget::continueShuttingDownBecauseDismounted()
{
    handleD("we're about to dispatch a shutdown... which we will receive once the OS sends it to us while shutting down every app");
    m_BusinessThread->quit();
    m_BusinessThread->wait();
    //QMetaObject::invokeMethod(m_Business, "shutdown", Qt::QueuedConnection);
    QProcess::startDetached("/usr/bin/sudo /sbin/poweroff");
}
void mainWidget::rigConnections()
{
    connect(m_Business, SIGNAL(d(const QString &)), this, SLOT(handleD(const QString &)));

    connect(m_MountTcVolumesButton, SIGNAL(clicked()), this, SLOT(handleMountBtnClicked()));
    connect(this, SIGNAL(mountRequested(bool)), m_Business, SLOT(mount(bool)));
    connect(m_Business, SIGNAL(mounted()), this, SLOT(handleMounted()));
    connect(m_DismountTcVolumesButton, SIGNAL(clicked()), m_Business, SLOT(dismount()));
    connect(m_Business, SIGNAL(dismounted()), this, SLOT(handleDismounted()));
    connect(m_MountIfNeededAndCommitButton, SIGNAL(clicked()), this, SLOT(handleCommitBtnClicked()));
    connect(this, SIGNAL(commitRequested(const QString &)), m_Business, SLOT(commit(const QString &)));
    connect(m_Business, SIGNAL(committed()), this, SLOT(handleCommitComplete()));
    connect(m_ShutdownAfterDismountButton, SIGNAL(clicked()), this, SLOT(handleShutdownBtnClicked()));
}
void mainWidget::buildGui()
{
    m_Debug = new QPlainTextEdit();

    m_MountAsReadOnly = new QCheckBox("Mount As Read-Only");

    m_MountTcVolumesButton = new QPushButton("Mount Truecrypt Volumes");
    m_DismountTcVolumesButton = new QPushButton("Dismount Truecrypt Volumes");
    updateGuiBasedOnMountedOrDismounted();

    m_GitCommitMessage = new QLineEdit(QString(AUTO_COMMIT_DATE_REPLACE_SPECIAL_TXT));

    m_MountIfNeededAndCommitButton = new QPushButton("Commit. Mount if Needed");
    m_ShutdownAfterDismountButton = new QPushButton("Shutdown After Dismount");


    m_Layout = new QVBoxLayout();
    m_Layout->addWidget(m_MountAsReadOnly);

    QHBoxLayout *mountDismountButtonsLayout = new QHBoxLayout();
    mountDismountButtonsLayout->addWidget(m_MountTcVolumesButton);
    mountDismountButtonsLayout->addWidget(m_DismountTcVolumesButton);

    m_Layout->addLayout(mountDismountButtonsLayout);

    m_Layout->addWidget(m_GitCommitMessage);

    QHBoxLayout *commitAndShutdownLayout = new QHBoxLayout();
    commitAndShutdownLayout->addWidget(m_MountIfNeededAndCommitButton);
    commitAndShutdownLayout->addWidget(m_ShutdownAfterDismountButton);

    m_Layout->addLayout(commitAndShutdownLayout);

    m_Layout->addWidget(m_Debug);

    this->setLayout(m_Layout);
}
void mainWidget::initialize()
{
    connect(m_Business, SIGNAL(initializedAndProbed(bool,bool)), this, SLOT(handleBusinessInitializedAndProbed(bool,bool)));

    m_BusinessThread = new QThread();
    m_Business->moveToThread(m_BusinessThread);
    m_BusinessThread->start();
    QMetaObject::invokeMethod(m_Business, "initializeAndProbe", Qt::QueuedConnection);
}
void mainWidget::handleBusinessInitializedAndProbed(bool fucked, bool theyAreMounted)
{
    m_Mounted = theyAreMounted;

    buildGui();

    if(fucked)
    {
        handleD("Shit Is Fucked. This usually means one is mounted and the other isn't. Remedy this in TC GUI then re-launch app. Disabling all buttons for this run...");
    }
    else
    {
        rigConnections();
    }
}
void mainWidget::handleMountBtnClicked()
{
    emit mountRequested(m_MountAsReadOnly->isChecked());
}
void mainWidget::handleMounted()
{
    m_Mounted = true;
    if(m_AboutToCommitWaitingForMount)
    {
        m_AboutToCommitWaitingForMount = false;
        continueCommitProcessBecauseMounted();
    }
    handleD("We Are Mounted");
    updateGuiBasedOnMountedOrDismounted();
}
void mainWidget::handleDismounted()
{
    handleD("We Are Dis-Mounted");
    m_Mounted = false;
    updateGuiBasedOnMountedOrDismounted();
    if(m_AboutToShutdownWaitingForDismount)
    {
        m_AboutToShutdownWaitingForDismount = false;
        continueShuttingDownBecauseDismounted();
    }
}
void mainWidget::handleCommitBtnClicked()
{
    m_MountIfNeededAndCommitButton->setEnabled(false); //so they don't click again
    if(m_MountAsReadOnly->isChecked())
    {
        handleD("Un-Check Read-Only And Try Again...");
    }
    else if(m_GitCommitMessage->text().trimmed().isEmpty())
    {
        handleD("Can't commit with empty message. Put in \"" + QString(AUTO_COMMIT_DATE_REPLACE_SPECIAL_TXT) + "\" if you want the auto-commit message");
    }
    else
    {
        m_Committing = true;
        if(!m_Mounted)
        {
            m_AboutToCommitWaitingForMount = true;
            emit mountRequested(false);
        }
        else
        {
            continueCommitProcessBecauseMounted();
        }
    }
}
void mainWidget::handleCommitComplete()
{
    handleD("Commit Completed. Note that we are still mounted as ReadWrite"); //TODOopt: add 'dismount after commit' checkbox next to 'mount as read only'
    m_Committing = false;
    m_MountIfNeededAndCommitButton->setEnabled(true);
}
void mainWidget::handleShutdownBtnClicked()
{
    if(m_Committing)
    {
        handleD("We are in the middle of a commit. Fuck off");
    }
    else
    {
        bool backendIsBusy = true;

        QMetaObject::invokeMethod(m_Business, "isBusyWithSomething", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, backendIsBusy));

        if(!backendIsBusy)
        {
            if(m_Mounted)
            {
                m_AboutToShutdownWaitingForDismount = true;
                QMetaObject::invokeMethod(m_Business, "dismount", Qt::QueuedConnection);
            }
            else
            {
                continueShuttingDownBecauseDismounted();
            }
        }
        else
        {
            handleD("backend is busy with something so we won't even attempt to shut down");
        }
    }
}
void mainWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
